'use strict';

const common = require('../common');
const assert = require('assert');
const net = require('net');

process.chdir(common.fixturesDir);
const repl = require('repl');

const server = net.createServer((conn) => {
  repl.start('', conn).on('exit', () => {
    conn.destroy();
    server.close();
  });
});

const host = common.localhostIPv4;
const port = 0;
const options = { host, port };

let answer = '';
server.listen(options, function() {
  options.port = this.address().port;
  const conn = net.connect(options);
  let cmds = [
    'require("baz")',
    'require("./baz")',
    '.exit'
  ];

  const run = ([cmd, ...rest]) => {
    if (!cmd) return;
    cmds = rest;
    conn.write(`${cmd}\n`);
  };

  conn.setEncoding('utf8');
  conn.on('data', (data) => {
    if (data !== '> ') {
      setTimeout(() => run(cmds), 100);
    }
    answer += data;
  });
  run(cmds);
});

process.on('exit', function() {
  assert.strictEqual(false, /Cannot find module/.test(answer));
  assert.strictEqual(false, /Error/.test(answer));
  assert.strictEqual(answer, '\'eye catcher\'\n\'perhaps I work\'\n');
});
