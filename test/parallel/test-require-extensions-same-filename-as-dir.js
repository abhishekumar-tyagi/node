'use strict';
const common = require('../common');
const assert = require('assert');

var content = require(common.fixturesDir +
  '/json-with-directory-name-module/module-stub/one/two/three.js');

assert.notEqual(content.rocko, 'artischocko');
assert.equal(content, 'hello from module-stub!');
