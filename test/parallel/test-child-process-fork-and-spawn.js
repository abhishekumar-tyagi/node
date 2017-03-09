'use strict';
const common = require('../common');
const assert = require('assert');
const spawn = require('child_process').spawn;
const fork = require('child_process').fork;

// Fork, then spawn. The spawned process should not hang.
switch (process.argv[2] || '') {
  case '':
    fork(__filename, ['fork']).on('exit', common.mustCall(checkExit));
    break;
  case 'fork':
    spawn(process.execPath, [__filename, 'spawn'])
      .on('exit', common.mustCall(checkExit));
    break;
  case 'spawn':
    break;
  default:
    assert(0);
}

function checkExit(statusCode) {
  assert.equal(statusCode, 0);
  process.nextTick(process.exit);
}
