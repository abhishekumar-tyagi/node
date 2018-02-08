'use strict';
const common = require('../common');
const assert = require('assert');

common.expectWarning(
  'DeprecationWarning',
  'process.assert() is deprecated. Use the `assert` module instead.'
);

assert.strictEqual(process.assert(1, 'error'), undefined);
common.expectsError(() => {
  process.assert(undefined, 'errorMessage');
}, {
  code: 'ERR_ASSERTION',
  type: Error,
  message: 'errorMessage'
});
common.expectsError(() => {
  process.assert(false);
}, {
  code: 'ERR_ASSERTION',
  type: Error,
  message: 'assertion error'
});
