'use strict';
const assert = require('assert');
const common = require('../common');

var errs = 0;

process.stdin.resume();
process.stdin._handle.close();
process.stdin._handle.unref();  // Should not segfault.
process.stdin.on('error', function(err) {
  errs++;
});

process.on('exit', function() {
  assert.strictEqual(errs, 1);
});
