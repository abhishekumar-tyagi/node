// Flags: --experimental-worker
'use strict';
const common = require('../common');
const assert = require('assert');
const { Worker } = require('worker');

// Do not use isMainThread so that this test itself can be run inside a Worker.
if (!process.env.HAS_STARTED_WORKER) {
  process.env.HAS_STARTED_WORKER = 1;
  const w = new Worker(__filename);
  w.on('message', common.mustNotCall());
  w.on('error', common.mustCall((err) => {
    // TODO(addaleax): be more specific here
    assert(/foo/.test(err));
  }));
} else {
  throw new Error('foo');
}
