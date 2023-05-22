// Flags: --experimental-permission --allow-env --allow-fs-read=*
'use strict';

const common = require('../common');
common.skipIfWorker();
const assert = require('assert');
const childProcess = require('child_process');

if (process.argv[2] === 'child') {
  process.exit(0);
}

// Guarantee the initial state
{
  assert.ok(!process.permission.has('child'));
}

// When a permission is set by cli, the process shouldn't be able
// to spawn
{
  assert.throws(() => {
    childProcess.spawn(process.execPath, ['--version']);
  }, common.expectsError({
    code: 'ERR_ACCESS_DENIED',
    permission: 'ChildProcess',
  }));
  assert.throws(() => {
    childProcess.spawnSync(process.execPath, ['--version']);
  }, common.expectsError({
    code: 'ERR_ACCESS_DENIED',
    permission: 'ChildProcess',
  }));
  assert.throws(() => {
    childProcess.exec(process.execPath, ['--version']);
  }, common.expectsError({
    code: 'ERR_ACCESS_DENIED',
    permission: 'ChildProcess',
  }));
  assert.throws(() => {
    childProcess.execSync(process.execPath, ['--version']);
  }, common.expectsError({
    code: 'ERR_ACCESS_DENIED',
    permission: 'ChildProcess',
  }));
  assert.throws(() => {
    childProcess.fork(__filename, ['child']);
  }, common.expectsError({
    code: 'ERR_ACCESS_DENIED',
    permission: 'ChildProcess',
  }));
  assert.throws(() => {
    childProcess.execFile(process.execPath, ['--version']);
  }, common.expectsError({
    code: 'ERR_ACCESS_DENIED',
    permission: 'ChildProcess',
  }));
  assert.throws(() => {
    childProcess.execFileSync(process.execPath, ['--version']);
  }, common.expectsError({
    code: 'ERR_ACCESS_DENIED',
    permission: 'ChildProcess',
  }));
}
