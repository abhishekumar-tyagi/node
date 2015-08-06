'use strict';
const common = require('../common');
const assert = require('assert');

const spawn = require('child_process').spawn;

var exitCode;
var termSignal;
var gotStdoutEOF = false;
var gotStderrEOF = false;

var cat = spawn(common.isWindows ? 'cmd' : 'cat');


cat.stdout.on('end', function() {
  gotStdoutEOF = true;
});

cat.stderr.on('data', function(chunk) {
  assert.ok(false);
});

cat.stderr.on('end', function() {
  gotStderrEOF = true;
});

cat.on('exit', function(code, signal) {
  exitCode = code;
  termSignal = signal;
});

assert.equal(cat.killed, false);
cat.kill();
assert.equal(cat.killed, true);

process.on('exit', function() {
  assert.strictEqual(exitCode, null);
  assert.strictEqual(termSignal, 'SIGTERM');
  assert.ok(gotStdoutEOF);
  assert.ok(gotStderrEOF);
});
