'use strict';
const common = require('../common');
const assert = require('assert');
const { Worker, resourceLimits, isMainThread } = require('worker_threads');
const { allocateUntilCrash } = require('../common/allocate-and-check-limits');

if (isMainThread) {
  assert.deepStrictEqual(resourceLimits, {});
}

const testResourceLimits = {
  maxOldGenerationSizeMb: 16,
  maxYoungGenerationSizeMb: 4,
  codeRangeSizeMb: 16,
  stackSizeMb: 1,
};

// Do not use isMainThread so that this test itself can be run inside a Worker.
if (!process.env.HAS_STARTED_WORKER) {
  process.env.HAS_STARTED_WORKER = 1;
  const w = new Worker(__filename, { resourceLimits: testResourceLimits });
  assert.deepStrictEqual(w.resourceLimits, testResourceLimits);
  w.on('exit', common.mustCall((code) => {
    assert.strictEqual(code, 1);
    assert.deepStrictEqual(w.resourceLimits, {});
  }));
  w.on('error', common.expectsError({
    code: 'ERR_WORKER_OUT_OF_MEMORY',
    message: 'Worker terminated due to reaching memory limit: ' +
    'JS heap out of memory'
  }));
  return;
}

assert.deepStrictEqual(resourceLimits, testResourceLimits);
allocateUntilCrash(resourceLimits);
