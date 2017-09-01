// Copyright Joyent, Inc. and other Node contributors.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so, subject to the
// following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
// NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
// USE OR OTHER DEALINGS IN THE SOFTWARE.

'use strict';
const common = require('../common');

if (!common.hasCrypto)
  common.skip('missing crypto');

const assert = require('assert');
const crypto = require('crypto');

crypto.DEFAULT_ENCODING = 'buffer';

// bump, we register a lot of exit listeners
process.setMaxListeners(256);

const expectedErrorRegexp = /^TypeError: size must be a number >= 0$/;
[crypto.randomBytes, crypto.pseudoRandomBytes].forEach(function(f) {
  [-1, undefined, null, false, true, {}, []].forEach(function(value) {
    assert.throws(function() { f(value); }, expectedErrorRegexp);
    assert.throws(function() { f(value, common.mustNotCall()); },
                  expectedErrorRegexp);
  });

  [0, 1, 2, 4, 16, 256, 1024].forEach(function(len) {
    f(len, common.mustCall(function(ex, buf) {
      assert.strictEqual(ex, null);
      assert.strictEqual(buf.length, len);
      assert.ok(Buffer.isBuffer(buf));
    }));
  });
});

{
  const buf = Buffer.alloc(10);
  const before = buf.toString('hex');
  const after = crypto.randomFillSync(buf).toString('hex');
  assert.notStrictEqual(before, after);
}

{
  const buf = new Uint8Array(new Array(10).fill(0));
  const before = Buffer.from(buf).toString('hex');
  crypto.randomFillSync(buf);
  const after = Buffer.from(buf).toString('hex');
  assert.notStrictEqual(before, after);
}

{
  const buf = Buffer.alloc(10);
  const before = buf.toString('hex');
  crypto.randomFill(buf, common.mustCall((err, buf) => {
    assert.ifError(err);
    const after = buf.toString('hex');
    assert.notStrictEqual(before, after);
  }));
}

{
  const buf = new Uint8Array(new Array(10).fill(0));
  const before = Buffer.from(buf).toString('hex');
  crypto.randomFill(buf, common.mustCall((err, buf) => {
    assert.ifError(err);
    const after = Buffer.from(buf).toString('hex');
    assert.notStrictEqual(before, after);
  }));
}

{
  const buf = Buffer.alloc(10);
  const before = buf.toString('hex');
  crypto.randomFillSync(buf, 5, 5);
  const after = buf.toString('hex');
  assert.notStrictEqual(before, after);
  assert.deepStrictEqual(before.slice(0, 5), after.slice(0, 5));
}

{
  const buf = new Uint8Array(new Array(10).fill(0));
  const before = Buffer.from(buf).toString('hex');
  crypto.randomFillSync(buf, 5, 5);
  const after = Buffer.from(buf).toString('hex');
  assert.notStrictEqual(before, after);
  assert.deepStrictEqual(before.slice(0, 5), after.slice(0, 5));
}

{
  const buf = Buffer.alloc(10);
  const before = buf.toString('hex');
  crypto.randomFillSync(buf, 5);
  const after = buf.toString('hex');
  assert.notStrictEqual(before, after);
  assert.deepStrictEqual(before.slice(0, 5), after.slice(0, 5));
}

{
  const buf = Buffer.alloc(10);
  const before = buf.toString('hex');
  crypto.randomFill(buf, 5, 5, common.mustCall((err, buf) => {
    assert.ifError(err);
    const after = buf.toString('hex');
    assert.notStrictEqual(before, after);
    assert.deepStrictEqual(before.slice(0, 5), after.slice(0, 5));
  }));
}

{
  const buf = new Uint8Array(new Array(10).fill(0));
  const before = Buffer.from(buf).toString('hex');
  crypto.randomFill(buf, 5, 5, common.mustCall((err, buf) => {
    assert.ifError(err);
    const after = Buffer.from(buf).toString('hex');
    assert.notStrictEqual(before, after);
    assert.deepStrictEqual(before.slice(0, 5), after.slice(0, 5));
  }));
}

{
  const bufs = [
    Buffer.alloc(10),
    new Uint8Array(new Array(10).fill(0))
  ];

  const errMessages = {
    offsetOutOfBounds: '"offset" is outside of buffer bounds',
    sizeOutOfBounds: '"size + offset" is outside of buffer bounds',
    sizeOutOfRange: (size) => {
      return 'The value of "size" must be <= 4294967295 and >= 0. ' +
        `Received "${size}"`;
    },
    offsetOutOfRange: (offset) => {
      return 'The value of "offset" must be <= 4294967295 and >= 0. ' +
        `Received "${offset}"`;
    }
  };

  const max = require('buffer').kMaxLength + 1;

  const argTypeErr = {
    code: 'ERR_INVALID_ARG_TYPE',
    type: TypeError
  };

  for (const buf of bufs) {
    const len = Buffer.byteLength(buf);
    assert.strictEqual(len, 10, `Expected byteLength of 10, got ${len}`);

    common.expectsError(
      () => {
        crypto.randomFillSync(buf, 'test');
      },
      argTypeErr
    );

    common.expectsError(
      () => {
        crypto.randomFillSync(buf, NaN);
      },
      argTypeErr
    );

    common.expectsError(
      () => {
        crypto.randomFill(buf, 'test', common.mustNotCall());
      },
      argTypeErr
    );

    common.expectsError(
      () => {
        crypto.randomFill(buf, NaN, common.mustNotCall());
      },
      argTypeErr
    );

    common.expectsError(
      () => {
        crypto.randomFillSync(buf, 11);
      },
      {
        code: 'ERR_BUFFER_OUT_OF_BOUNDS',
        message: errMessages.offsetOutOfBounds,
        type: RangeError
      }
    );

    common.expectsError(
      () => {
        crypto.randomFillSync(buf, max);
      },
      {
        code: 'ERR_BUFFER_OUT_OF_BOUNDS',
        message: errMessages.offsetOutOfBounds,
        type: RangeError
      }
    );

    common.expectsError(
      () => {
        crypto.randomFill(buf, 11, common.mustNotCall());
      },
      {
        code: 'ERR_BUFFER_OUT_OF_BOUNDS',
        message: errMessages.offsetOutOfBounds,
        type: RangeError
      }
    );

    common.expectsError(
      () => {
        crypto.randomFill(buf, max, common.mustNotCall());
      },
      {
        code: 'ERR_BUFFER_OUT_OF_BOUNDS',
        message: errMessages.offsetOutOfBounds,
        type: RangeError
      }
    );

    common.expectsError(
      () => {
        crypto.randomFillSync(buf, 0, 'test');
      },
      argTypeErr
    );

    common.expectsError(
      () => {
        crypto.randomFillSync(buf, 0, NaN);
      },
      argTypeErr
    );

    common.expectsError(
      () => {
        crypto.randomFill(buf, 0, 'test', common.mustNotCall());
      },
      argTypeErr
    );

    common.expectsError(
      () => {
        crypto.randomFill(buf, 0, NaN, common.mustNotCall());
      },
      argTypeErr
    );

    {
      const size = (-1 >>> 0) + 1;

      common.expectsError(
        () => {
          crypto.randomFillSync(buf, 0, -10);
        },
        {
          code: 'ERR_VALUE_OUT_OF_RANGE',
          message: errMessages.sizeOutOfRange(-10),
          type: TypeError
        }
      );

      common.expectsError(
        () => {
          crypto.randomFillSync(buf, 0, size);
        },
        {
          code: 'ERR_VALUE_OUT_OF_RANGE',
          message: errMessages.sizeOutOfRange(size),
          type: TypeError
        }
      );

      common.expectsError(
        () => {
          crypto.randomFill(buf, 0, -10, common.mustNotCall());
        },
        {
          code: 'ERR_VALUE_OUT_OF_RANGE',
          message: errMessages.sizeOutOfRange(-10),
          type: TypeError
        }
      );

      common.expectsError(
        () => {
          crypto.randomFill(buf, 0, size, common.mustNotCall());
        },
        {
          code: 'ERR_VALUE_OUT_OF_RANGE',
          message: errMessages.sizeOutOfRange(size),
          type: TypeError
        }
      );
    }

    common.expectsError(
      () => {
        crypto.randomFillSync(buf, -10);
      },
      {
        code: 'ERR_VALUE_OUT_OF_RANGE',
        message: errMessages.offsetOutOfRange(-10),
        type: TypeError
      }
    );

    common.expectsError(
      () => {
        crypto.randomFill(buf, -10, common.mustNotCall());
      },
      {
        code: 'ERR_VALUE_OUT_OF_RANGE',
        message: errMessages.offsetOutOfRange(-10),
        type: TypeError
      }
    );

    common.expectsError(
      () => {
        crypto.randomFillSync(buf, 1, 10);
      },
      {
        code: 'ERR_BUFFER_OUT_OF_BOUNDS',
        message: errMessages.sizeOutOfBounds,
        type: RangeError
      }
    );

    common.expectsError(
      () => {
        crypto.randomFill(buf, 1, 10, common.mustNotCall());
      },
      {
        code: 'ERR_BUFFER_OUT_OF_BOUNDS',
        message: errMessages.sizeOutOfBounds,
        type: RangeError
      }
    );

    common.expectsError(
      () => {
        crypto.randomFillSync(buf, 0, 12);
      },
      {
        code: 'ERR_BUFFER_OUT_OF_BOUNDS',
        message: errMessages.sizeOutOfBounds,
        type: RangeError
      }
    );

    common.expectsError(
      () => {
        crypto.randomFill(buf, 0, 12, common.mustNotCall());
      },
      {
        code: 'ERR_BUFFER_OUT_OF_BOUNDS',
        message: errMessages.sizeOutOfBounds,
        type: RangeError
      }
    );

    common.expectsError(
      () => {
        crypto.randomFill(buf, 'test');
      },
      {
        code: 'ERR_INVALID_CALLBACK',
        type: TypeError
      }
    );

    {
      // Offset is too big
      const offset = (-1 >>> 0) + 1;
      common.expectsError(
        () => {
          crypto.randomFillSync(buf, offset, 10);
        },
        {
          code: 'ERR_VALUE_OUT_OF_RANGE',
          message: errMessages.offsetOutOfRange(offset),
          type: TypeError
        }
      );

      common.expectsError(
        () => {
          crypto.randomFill(buf, offset, 10, common.mustNotCall());
        },
        {
          code: 'ERR_VALUE_OUT_OF_RANGE',
          message: errMessages.offsetOutOfRange(offset),
          type: TypeError
        }
      );
    }
  }
}

// test randomFill buf type exception
{
  common.expectsError(
    () => {
      crypto.randomFillSync(1, 'test');
    },
    {
      code: 'ERR_INVALID_ARG_TYPE',
      type: TypeError
    }
  );
  common.expectsError(
    () => {
      crypto.randomFill(1, 'test');
    },
    {
      code: 'ERR_INVALID_ARG_TYPE',
      type: TypeError
    }
  );
}

// #5126, "FATAL ERROR: v8::Object::SetIndexedPropertiesToExternalArrayData()
// length exceeds max acceptable value"
assert.throws(function() {
  crypto.randomBytes((-1 >>> 0) + 1);
}, /^TypeError: size must be a number >= 0$/);
