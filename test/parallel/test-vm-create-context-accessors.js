'use strict';
const common = require('../common');
const assert = require('assert');
const vm = require('vm');

var ctx = {};

Object.defineProperty(ctx, 'getter', {
  get: function() {
    return 'ok';
  }
});

var val;
Object.defineProperty(ctx, 'setter', {
  set: function(_val) {
    val = _val;
  },
  get: function() {
    return 'ok=' + val;
  }
});

ctx = vm.createContext(ctx);

var result = vm.runInContext('setter = "test";[getter,setter]', ctx);
assert.deepEqual(result, ['ok', 'ok=test']);
