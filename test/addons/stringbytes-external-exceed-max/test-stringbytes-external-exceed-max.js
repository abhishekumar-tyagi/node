'use strict';

const common = require('../../common');
const binding = require('./build/Release/binding');
const assert = require('assert');

const skipMessage = 'intensive toString tests due to memory confinements';
if (!common.enoughTestMem) {
  common.skip(skipMessage);
  return;
}

// v8 fails silently if string length > v8::String::kMaxLength
// v8::String::kMaxLength defined in v8.h
const kStringMaxLength = process.binding('buffer').kStringMaxLength;

try {
  var buf = new Buffer(kStringMaxLength * 2 + 2);
} catch (e) {
  // If the exception is not due to memory confinement then rethrow it.
  if (e.message !== 'Invalid array buffer length') throw (e);
  common.skip(skipMessage);
  return;
}

// Ensure we have enough memory available for future allocations to succeed.
if (!binding.ensureAllocation(2 * kStringMaxLength)) {
  common.skip(skipMessage);
  return;
}

assert.throws(function() {
  buf.toString('utf16le');
}, /toString failed/);
