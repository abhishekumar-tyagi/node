'use strict';

const common = require('../common');
const tmpdir = require('../common/tmpdir');
const assert = require('assert');
const spawn = require('child_process').spawn;
const path = require('path');

tmpdir.refresh();

const requirePath = path.join(tmpdir.path, 'non-existent.json');

// Use -i to force node into interactive mode, despite stdout not being a TTY
const child = spawn(process.execPath, ['-i']);

let out = '';
const input = `try { require('${requirePath}'); } catch {} ` +
              `require('fs').writeFileSync('${requirePath}', '1');` +
              `require('${requirePath}');`;

child.stderr.on('data', common.mustNotCall());

child.stdout.setEncoding('utf8');
child.stdout.on('data', (c) => {
  out += c;
});
child.stdout.on('end', common.mustCall(() => {
  assert.strictEqual(out, '> 1\n> ');
}));

child.stdin.end(input);
