'use strict';
const common = require('../common');
const assert = require('assert');
const { spawnSync } = require('child_process');
const { setTimeout } = require('timers/promises');

if (process.argv[2] === 'child') {
  const test = require('node:test');

  if (process.argv[3] === 'abortSignal') {
    assert.throws(() => test({ signal: {} }), {
      code: 'ERR_INVALID_ARG_TYPE',
      name: 'TypeError'
    });

    let testSignal;
    test({ timeout: 10 }, common.mustCall(async ({ signal }) => {
      assert.strictEqual(signal.aborted, false);
      testSignal = signal;
      await setTimeout(50);
    })).finally(common.mustCall(() => {
      test(() => assert.strictEqual(testSignal.aborted, true));
    });
  } else assert.fail('unreachable');
} else {
  const child = spawnSync(process.execPath, [__filename, 'child', 'abortSignal']);
  const stdout = child.stdout.toString();
  assert.match(stdout, /# pass 1/);
  assert.match(stdout, /# fail 0/);
  assert.match(stdout, /# cancelled 1/);
  assert.strictEqual(child.status, 1);
  assert.strictEqual(child.signal, null);
}
