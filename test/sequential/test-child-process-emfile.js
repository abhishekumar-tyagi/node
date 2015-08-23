'use strict';
const common = require('../common');
const assert = require('assert');
const spawn = require('child_process').spawn;
const fs = require('fs');

if (common.isWindows) {
  console.log('1..0 # Skipped: no RLIMIT_NOFILE on Windows');
  return;
}

for (;;) {
  try {
    fs.openSync(__filename, 'r');
  } catch (err) {
    assert(err.code === 'EMFILE' || err.code === 'ENFILE');
    break;
  }
}

// Should emit an error, not throw.
const proc = spawn(process.execPath, ['-e', '0']);

proc.on('error', common.mustCall(function(err) {
  assert(err.code === 'EMFILE' || err.code === 'ENFILE');
}));

// 'exit' should not be emitted, the process was never spawned.
proc.on('exit', assert.fail);
