var common = require('../common');
var assert = require('assert');

common.globalCheck = false;
global.gc = 42;  // Not a valid global unless --expose_gc is set.
assert.deepEqual(common.leakedGlobals(), ['gc']);
