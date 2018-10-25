'use strict';

const common = require('../common');
const assert = require('assert');

const b = Buffer.alloc(1, 'a');
const c = Buffer.alloc(1, 'c');
const d = Buffer.alloc(2, 'aa');
const e = new Uint8Array([ 0x61, 0x61 ]); // ASCII 'aa', same as d

assert.strictEqual(b.compare(c), -1);
assert.strictEqual(c.compare(d), 1);
assert.strictEqual(d.compare(b), 1);
assert.strictEqual(d.compare(e), 0);
assert.strictEqual(b.compare(d), -1);
assert.strictEqual(b.compare(b), 0);

assert.strictEqual(Buffer.compare(b, c), -1);
assert.strictEqual(Buffer.compare(c, d), 1);
assert.strictEqual(Buffer.compare(d, b), 1);
assert.strictEqual(Buffer.compare(b, d), -1);
assert.strictEqual(Buffer.compare(c, c), 0);
assert.strictEqual(Buffer.compare(e, e), 0);
assert.strictEqual(Buffer.compare(d, e), 0);
assert.strictEqual(Buffer.compare(d, b), 1);

assert.strictEqual(Buffer.compare(Buffer.alloc(0), Buffer.alloc(0)), 0);
assert.strictEqual(Buffer.compare(Buffer.alloc(0), Buffer.alloc(1)), -1);
assert.strictEqual(Buffer.compare(Buffer.alloc(1), Buffer.alloc(0)), 1);

assert.throws(() => Buffer.compare(Buffer.alloc(1), 'abc'), {
  code: 'ERR_INVALID_ARG_TYPE',
  message: 'The "buf2" argument must be one of type Buffer or Uint8Array. ' +
           'Received type string'
});
assert.throws(() => Buffer.compare('abc', Buffer.alloc(1)), {
  code: 'ERR_INVALID_ARG_TYPE',
  message: 'The "buf1" argument must be one of type Buffer or Uint8Array. ' +
           'Received type string'
});

common.expectsError(() => Buffer.alloc(1).compare('abc'), {
  code: 'ERR_INVALID_ARG_TYPE',
  type: TypeError,
  message: 'The "target" argument must be one of ' +
           'type Buffer or Uint8Array. Received type string'
});


// Test compare with offset
{
  const a = Buffer.from([1, 2, 3, 4, 5, 6, 7, 8, 9, 0]);
  const b = Buffer.from([5, 6, 7, 8, 9, 0, 1, 2, 3, 4]);

  assert.strictEqual(a.compare(b), -1);

// Equivalent to a.compare(b).
  assert.strictEqual(a.compare(b, 0), -1);
  assert.strictEqual(a.compare(b, '0'), -1);
  assert.strictEqual(a.compare(b, undefined), -1);

// Equivalent to a.compare(b).
  assert.strictEqual(a.compare(b, 0, undefined, 0), -1);

// Zero-length target, return 1
  assert.strictEqual(a.compare(b, 0, 0, 0), 1);
  assert.strictEqual(a.compare(b, '0', '0', '0'), 1);

// Equivalent to Buffer.compare(a, b.slice(6, 10))
  assert.strictEqual(a.compare(b, 6, 10), 1);

// Zero-length source, return -1
  assert.strictEqual(a.compare(b, 6, 10, 0, 0), -1);

// Zero-length source and target, return 0
  assert.strictEqual(a.compare(b, 0, 0, 0, 0), 0);
  assert.strictEqual(a.compare(b, 1, 1, 2, 2), 0);

// Equivalent to Buffer.compare(a.slice(4), b.slice(0, 5))
  assert.strictEqual(a.compare(b, 0, 5, 4), 1);

// Equivalent to Buffer.compare(a.slice(1), b.slice(5))
  assert.strictEqual(a.compare(b, 5, undefined, 1), 1);

// Equivalent to Buffer.compare(a.slice(2), b.slice(2, 4))
  assert.strictEqual(a.compare(b, 2, 4, 2), -1);

// Equivalent to Buffer.compare(a.slice(4), b.slice(0, 7))
  assert.strictEqual(a.compare(b, 0, 7, 4), -1);

// Equivalent to Buffer.compare(a.slice(4, 6), b.slice(0, 7));
  assert.strictEqual(a.compare(b, 0, 7, 4, 6), -1);

// zero length target
  assert.strictEqual(a.compare(b, 0, null), 1);

// coerces to targetEnd == 5
  assert.strictEqual(a.compare(b, 0, { valueOf: () => 5 }), -1);

// zero length target
  assert.strictEqual(a.compare(b, Infinity, -Infinity), 1);

// zero length target because default for targetEnd <= targetSource
  assert.strictEqual(a.compare(b, '0xff'), 1);

  const oor = common.expectsError({ code: 'ERR_OUT_OF_RANGE' }, 7);

  assert.throws(() => a.compare(b, 0, 100, 0), oor);
  assert.throws(() => a.compare(b, 0, 1, 0, 100), oor);
  assert.throws(() => a.compare(b, -1), oor);
  assert.throws(() => a.compare(b, 0, '0xff'), oor);
  assert.throws(() => a.compare(b, 0, Infinity), oor);
  assert.throws(() => a.compare(b, 0, 1, -1), oor);
  assert.throws(() => a.compare(b, -Infinity, Infinity), oor);
  common.expectsError(() => a.compare(), {
    code: 'ERR_INVALID_ARG_TYPE',
    type: TypeError,
    message: 'The "target" argument must be one of ' +
      'type Buffer or Uint8Array. Received type undefined'
  });

  // Validate arg type checking.
  // Refs: https://github.com/nodejs/node/issues/23668
  {
    const expected = {
      code: 'ERR_INVALID_ARG_TYPE',
      type: TypeError,
    }

    common.expectsError(
      () => b.compare(c, '1'),
      expected
    );
    common.expectsError(
      () => b.compare(c, 0, '1'),
      expected
    );
    common.expectsError(
      () => b.compare(c, 0, 0, '1'),
      expected
    );
    common.expectsError(
      () => b.compare(c, 0, 0, 0, '1'),
      expected
    );
  }

  // Extra! Validate arg range checking.
  // Refs: https://github.com/nodejs/node/issues/23668
  {
    const expected = {
      code: 'ERR_OUT_OF_RANGE',
      type: RangeError,
    }
    common.expectsError(
      () => b.copy(c, -1),
      expected
    );
    common.expectsError(
      () => b.copy(c, 0, -1),
      expected
    );
    common.expectsError(
      () => b.copy(c, 0, 0, -1),
      expected
    );
    common.expectsError(
      () => b.copy(c, 0, 0, 0, -1),
      expected
    );
  }
}
