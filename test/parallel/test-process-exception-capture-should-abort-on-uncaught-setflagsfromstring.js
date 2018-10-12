'use strict';
const common = require('../common');
const assert = require('assert');
const v8 = require('v8');

const sym = Symbol('foo');

assert.strictEqual(process.hasUncaughtExceptionCaptureCallback(), false);

v8.setFlagsFromString('--abort-on-uncaught-exception');
// This should make the process not crash even though the flag was passed.
process.setUncaughtExceptionCaptureCallback(sym, common.mustCall((err) => {
  assert.strictEqual(err.message, 'foo');
}));
throw new Error('foo');
