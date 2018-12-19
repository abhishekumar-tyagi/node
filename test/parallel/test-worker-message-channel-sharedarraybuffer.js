'use strict';

const common = require('../common');
if (!global.gc) {
  common.relaunchWithFlags(['--expose-gc', '--experimental-worker']);
}

const assert = require('assert');
const { Worker } = require('worker_threads');

{
  const sharedArrayBuffer = new SharedArrayBuffer(12);
  const local = Buffer.from(sharedArrayBuffer);

  const w = new Worker(`
    const { parentPort } = require('worker_threads');
    parentPort.on('message', ({ sharedArrayBuffer }) => {
      const local = Buffer.from(sharedArrayBuffer);
      local.write('world!', 6);
      parentPort.postMessage('written!');
    });
  `, { eval: true });
  w.on('message', common.mustCall(() => {
    assert.strictEqual(local.toString(), 'Hello world!');
    global.gc();
    w.terminate();
  }));
  w.postMessage({ sharedArrayBuffer });
  // This would be a race condition if the memory regions were overlapping
  local.write('Hello ');
}
