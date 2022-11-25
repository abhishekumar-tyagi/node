'use strict';
const common = require('../common');
const assert = require('assert');
const { Worker, isMainThread } = require('worker_threads');

if (isMainThread) {
  const workerData = new Int32Array(new SharedArrayBuffer(4));
  const w = new Worker(__filename, {
    workerData,
  });
  w.on('exit', common.mustCall(() => {
    assert.strictEqual(workerData[0], 0);
  }));
} else {
  const { workerData } = require('worker_threads');
  try {
    process.exit();
    throw new Error('xxx');
    // eslint-disable-next-line no-unused-vars
  } catch (err) {
    workerData[0] = 1;
  }
}
