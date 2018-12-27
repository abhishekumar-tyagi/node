'use strict';

const common = require('../../common');
common.requireFlags(['--expose-gc']);
const assert = require('assert');
const addon = require(`./build/${common.buildType}/binding`);

let obj1 = addon.createObject(10);
let obj2 = addon.createObject(20);
const result = addon.add(obj1, obj2);
assert.strictEqual(result, 30);

// Make sure the native destructor gets called.
obj1 = null;
obj2 = null;
global.gc();
assert.strictEqual(addon.finalizeCount(), 2);
