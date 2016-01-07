'use strict';
// Can't test this when 'make test' doesn't assign a tty to the stdout.
require('../common');
const assert = require('assert');

const shouldThrow = function() {
  process.stdout.end();
};

const validateError = function(e) {
  return e instanceof Error &&
    /^process.stdout cannot be closed/.test(e.message);
};

assert.throws(shouldThrow, validateError);
