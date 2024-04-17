'use strict';

const common = require('../common');
common.skipIfInspectorDisabled();

const { parentPort, isMainThread, Worker } = require('node:worker_threads');
const inspector = require('node:inspector');
const assert = require('node:assert');

// Refs: https://github.com/nodejs/node/issues/52467

let TIMEOUT = common.platformTimeout(5000);
if (common.isWindows) {
  // Refs: https://github.com/nodejs/build/issues/3014
  TIMEOUT = common.platformTimeout(15000);
}

(async () => {
  if (isMainThread) {
    // worker.terminate() should terminate the worker and the pending
    // inspector.waitForDebugger().
    {
      const worker = new Worker(__filename);
      await new Promise((r) => worker.on('message', r));
      await new Promise((r) => setTimeout(r, TIMEOUT));
      worker.on('exit', common.mustCall());
      await worker.terminate();
    }
    // process.exit() should kill the process.
    {
      const worker = new Worker(__filename);
      await new Promise((r) => worker.on('message', r));
      await new Promise((r) => setTimeout(r, TIMEOUT));
      process.on('exit', (status) => assert.strictEqual(status, 0));
      setImmediate(() => process.exit());
    }
  } else {
    inspector.open(0, undefined, false);
    parentPort.postMessage('open');
    inspector.waitForDebugger();
  }
})().then(common.mustCall());
