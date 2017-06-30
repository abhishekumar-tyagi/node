'use strict';

const common = require('../../common');
const binding = require(`./build/${common.buildType}/binding`);
const assert = require('assert');

const skipMessage = 'intensive toString tests due to memory confinements';
if (!common.enoughTestMem)
  common.skip(skipMessage);

// v8 fails silently if string length > v8::String::kMaxLength
// v8::String::kMaxLength defined in v8.h
const kStringMaxLength = process.binding('buffer').kStringMaxLength;

let buf;
try {
  buf = Buffer.allocUnsafe(kStringMaxLength + 1);
} catch (e) {
  // If the exception is not due to memory confinement then rethrow it.
  if (e.message !== 'Array buffer allocation failed') throw (e);
  common.skip(skipMessage);
}

// Ensure we have enough memory available for future allocations to succeed.
if (!binding.ensureAllocation(2 * kStringMaxLength))
  common.skip(skipMessage);

assert.throws(function() {
  buf.toString('hex');
}, /"toString\(\)" failed/);
