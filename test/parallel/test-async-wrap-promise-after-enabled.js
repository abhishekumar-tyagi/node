'use strict';

// Regression test for https://github.com/nodejs/node/issues/13237

const common = require('../common');
const assert = require('assert');

const async_hooks = require('async_hooks');

const seenEvents = [];

const p = new Promise((resolve) => resolve(1));
p.then(() => seenEvents.push('then'));

const hooks = async_hooks.createHook({
  before: common.mustCall((id) => {
    assert.ok(id > 1);
    seenEvents.push('before');
  }),

  after: common.mustCall((id) => {
    assert.ok(id > 1);
    seenEvents.push('after');
    hooks.disable();
  })
}).enable();

setImmediate(() => {
  assert.deepStrictEqual(seenEvents, ['before', 'then', 'after']);
});
