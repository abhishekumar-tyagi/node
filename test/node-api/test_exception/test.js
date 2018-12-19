'use strict';

const common = require('../../common');
if (!global.gc)
  common.relaunchWithFlags(['--expose-gc']);
const assert = require('assert');
const test_exception = require(`./build/${common.buildType}/test_exception`);

// Make sure that exceptions that occur during finalization are propagated.
function testFinalize(binding) {
  let x = test_exception[binding]();
  x = null;
  assert.throws(() => { global.gc(); }, /Error during Finalize/);

  // To assuage the linter's concerns.
  (function() {})(x);
}
testFinalize('createExternalBuffer');
