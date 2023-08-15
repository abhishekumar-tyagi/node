'use strict';

const common = require('../common');
const assert = require('assert');
const os = require('os');

if (process.argv[2] === 'child') {
  const { pipeline } = require('stream');
  pipeline(
    process.stdin,
    process.stdout,
    common.mustSucceed()
  );
} else {
  const cp = require('child_process');
  cp.exec('echo hello | "$NODE" "$FILE" child', {
    env: { NODE: process.execPath, FILE: __filename }
  }, common.mustSucceed((stdout) => {
    assert.strictEqual(stdout.split(os.EOL).shift().trim(), 'hello');
  }));
}
