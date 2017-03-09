'use strict';

const common = require('../../common');
const binding = require(`./build/${common.buildType}/binding`);
const assert = require('assert');

const skipMessage = 'intensive toString tests due to memory confinements';
if (!common.enoughTestMem) {
  common.skip(skipMessage);
  return;
}

// v8 fails silently if string length > v8::String::kMaxLength
// v8::String::kMaxLength defined in v8.h
const kStringMaxLength = process.binding('buffer').kStringMaxLength;

let buf;
try {
  buf = new Buffer(kStringMaxLength + 1);
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
  buf.toString('binary');
}, /toString failed/);

let maxString = buf.toString('binary', 1);
assert.equal(maxString.length, kStringMaxLength);
// Free the memory early instead of at the end of the next assignment
maxString = undefined;

maxString = buf.toString('binary', 0, kStringMaxLength);
assert.equal(maxString.length, kStringMaxLength);
