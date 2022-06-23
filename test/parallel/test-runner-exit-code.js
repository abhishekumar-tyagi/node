'use strict';
require('../common');
const assert = require('assert');
const { spawnSync } = require('child_process');

if (process.argv[2] === 'child') {
  const test = require('node:test');

  if (process.argv[3] === 'pass') {
    test('passing test', () => {
      assert.strictEqual(true, true);
    });
  } else if (process.argv[3] === 'fail') {
    assert.strictEqual(process.argv[3], 'fail');
    test('failing test', () => {
      assert.strictEqual(true, false);
    });
  } else if (process.argv[3] === 'never_ends') {
    assert.strictEqual(process.argv[3], 'never_ends');
    test('never ending test', () => {
      return new Promise(() => {});
    });
    process.kill(process.pid, 'SIGINT');
  }
} else {
  let child = spawnSync(process.execPath, [__filename, 'child', 'pass']);
  assert.strictEqual(child.status, 0);
  assert.strictEqual(child.signal, null);

  child = spawnSync(process.execPath, [__filename, 'child', 'fail']);
  assert.strictEqual(child.status, 1);
  assert.strictEqual(child.signal, null);

  child = spawnSync(process.execPath, [__filename, 'child', 'never_ends']);
  assert.strictEqual(child.status, 1);
  assert.strictEqual(child.signal, null);
  const stdout = child.stdout.toString();
  assert.match(stdout, /not ok 1 - never ending tes/);
  assert.match(stdout, /# cancelled 1/);
}
