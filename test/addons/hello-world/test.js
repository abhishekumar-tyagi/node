'use strict';
const common = require('../../common');
const assert = require('assert');
const binding = require(`./build/${common.buildType}/binding`);
assert.equal('world', binding.hello());
console.log('binding.hello() =', binding.hello());
