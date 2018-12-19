'use strict';

// This test ensures that userland-only AsyncResources cause a destroy event to
// be emitted when they get gced.

const common = require('../common');
if (!global.gc)
  common.relaunchWithFlags(['--expose-gc']);
const async_hooks = require('async_hooks');

const hook = async_hooks.createHook({
  destroy: common.mustCallAtLeast(2) // 1 immediate + manual destroy
}).enable();

{
  const res = new async_hooks.AsyncResource('foobar');
  res.emitDestroy();
}

setImmediate(() => {
  global.gc();
  setImmediate(() => {
    hook.disable();
  });
});
