'use strict';
const common = require('../common');
if (!common.hasCrypto)
  common.skip('missing crypto');

const assert = require('assert');
const crypto = require('crypto');

assert.strictEqual(
  crypto.timingSafeEqual(Buffer.from('foo'), Buffer.from('foo')),
  true,
  'should consider equal strings to be equal'
);

assert.strictEqual(
  crypto.timingSafeEqual(Uint8Array.of(1, 0), Uint16Array.of(1)),
  true,
  'should consider equal binary views to be equal' +
  ' (Uint8Array 1 0, Uint16Array 1)'
);

assert.strictEqual(
  crypto.timingSafeEqual(Uint8Array.of(0, 1), Uint16Array.of(1)),
  false,
  'should consider unequal binary views to be unequal' +
  ' (Uint8Array 0 1, Uint16Array 1)'
);

assert.strictEqual(
  crypto.timingSafeEqual(
    Uint8Array.of(1, 0, 0, 0, 0, 0, 0, 0),
    BigUint64Array.of(1n)
  ),
  true,
  'should consider equal binary views to be equal' +
  ' (Uint8Array 1 ...0, BigUint64Array 1n)'
);

assert.strictEqual(
  crypto.timingSafeEqual(
    Buffer.alloc(8, 255),
    BigInt64Array.of(-1n)
  ),
  true,
  'should consider equal binary views to be equal' +
  ' (Buffer 0xFF, BigInt64Array -1)'
);

assert.strictEqual(
  crypto.timingSafeEqual(
    new DataView(new ArrayBuffer(8)),
    BigInt64Array.of(0n)
  ),
  true,
  'should consider equal binary views to be equal' +
  ' (DataView, BigInt64Array)'
);

assert.strictEqual(
  crypto.timingSafeEqual(Buffer.from('foo'), Buffer.from('bar')),
  false,
  'should consider unequal strings to be unequal'
);

common.expectsError(
  () => crypto.timingSafeEqual(Buffer.from([1, 2, 3]), Buffer.from([1, 2])),
  {
    code: 'ERR_CRYPTO_TIMING_SAFE_EQUAL_LENGTH',
    type: RangeError,
    message: 'Input buffers must have the same length'
  }
);

common.expectsError(
  () => crypto.timingSafeEqual(
    Uint8Array.of(1, 2, 3),
    Uint16Array.of(1, 2, 3)
  ),
  {
    code: 'ERR_CRYPTO_TIMING_SAFE_EQUAL_LENGTH',
    type: RangeError,
    message: 'Input buffers must have the same length'
  }
);

common.expectsError(
  () => crypto.timingSafeEqual('not a buffer', Buffer.from([1, 2])),
  {
    code: 'ERR_INVALID_ARG_TYPE',
    type: TypeError,
    message:
      'The "buf1" argument must be one of type Buffer, TypedArray, or ' +
      'DataView. Received type string'
  }
);

common.expectsError(
  () => crypto.timingSafeEqual(Buffer.from([1, 2]), 'not a buffer'),
  {
    code: 'ERR_INVALID_ARG_TYPE',
    type: TypeError,
    message:
      'The "buf2" argument must be one of type Buffer, TypedArray, or ' +
      'DataView. Received type string'
  }
);
