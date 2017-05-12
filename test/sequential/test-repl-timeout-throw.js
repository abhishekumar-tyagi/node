'use strict';
const common = require('../common');
const assert = require('assert');

const spawn = require('child_process').spawn;

const child = spawn(process.execPath, [ '-i' ], {
  stdio: [null, null, 2]
});

let stdout = '';
child.stdout.setEncoding('utf8');
child.stdout.on('data', function(c) {
  process.stdout.write(c);
  stdout += c;
});

child.stdin.write = function(original) {
  return function(c) {
    process.stderr.write(c);
    return original.call(child.stdin, c);
  };
}(child.stdin.write);

child.stdout.once('data', function() {
  child.stdin.write('var throws = 0;');
  child.stdin.write('process.on("exit",function(){console.log(throws)});');
  child.stdin.write('function thrower(){console.log("THROW",throws++);XXX};');
  child.stdin.write('setTimeout(thrower);""\n');

  setTimeout(fsTest, 50);
  function fsTest() {
    const f = JSON.stringify(__filename);
    child.stdin.write(`fs.readFile(${f}, thrower);\n`);
    setTimeout(eeTest, 50);
  }

  function eeTest() {
    child.stdin.write(common.tagLFy`
      setTimeout(function() {
        const events = require('events');
        var e = new events.EventEmitter;
        process.nextTick(function() {
          e.on('x', thrower);
          setTimeout(function() {
            e.emit('x');
          });
        });
      });'';

    `);

    setTimeout(child.stdin.end.bind(child.stdin), common.platformTimeout(200));
  }
});

child.on('close', function(c) {
  assert.strictEqual(c, 0);
  // make sure we got 3 throws, in the end.
  const lastLine = stdout.trim().split(/\r?\n/).pop();
  assert.strictEqual(lastLine, '> 3');
});
