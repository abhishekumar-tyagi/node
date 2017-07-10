'use strict';

const common = require('../common');
const async_hooks = require('async_hooks');
const crypto = require('crypto');

const nestedHook = async_hooks.createHook({
  init: common.mustNotCall()
});
let nestedCall = false;

async_hooks.createHook({
  init: common.mustCall(function(id, type) {
    nestedHook.enable();
    if (!nestedCall) {
      nestedCall = true;
      crypto.randomBytes(1, common.mustCall());
    }
  }, 2)
}).enable();

crypto.randomBytes(1, common.mustCall());
