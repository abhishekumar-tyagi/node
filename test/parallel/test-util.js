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
// Flags: --expose-internals
const common = require('../common');
const assert = require('assert');
const util = require('util');
const errors = require('internal/errors');
const context = require('vm').runInNewContext;

// isArray
assert.strictEqual(util.isArray([]), true);
assert.strictEqual(util.isArray(Array()), true);
assert.strictEqual(util.isArray(new Array()), true);
assert.strictEqual(util.isArray(new Array(5)), true);
assert.strictEqual(util.isArray(new Array('with', 'some', 'entries')), true);
assert.strictEqual(util.isArray(context('Array')()), true);
assert.strictEqual(util.isArray({}), false);
assert.strictEqual(util.isArray({ push: function() {} }), false);
assert.strictEqual(util.isArray(/regexp/), false);
assert.strictEqual(util.isArray(new Error()), false);
assert.strictEqual(util.isArray({ __proto__: Array.prototype }), false);

// isDate
assert.strictEqual(util.isDate(new Date()), true);
assert.strictEqual(util.isDate(new Date(0), 'foo'), true);
assert.strictEqual(util.isDate(new (context('Date'))()), true);
assert.strictEqual(util.isDate(Date()), false);
assert.strictEqual(util.isDate({}), false);
assert.strictEqual(util.isDate([]), false);
assert.strictEqual(util.isDate(new Error()), false);
assert.strictEqual(util.isDate({ __proto__: Date.prototype }), false);

// isError
assert.strictEqual(util.isError(new Error()), true);
assert.strictEqual(util.isError(new TypeError()), true);
assert.strictEqual(util.isError(new SyntaxError()), true);
assert.strictEqual(util.isError(new (context('Error'))()), true);
assert.strictEqual(util.isError(new (context('TypeError'))()), true);
assert.strictEqual(util.isError(new (context('SyntaxError'))()), true);
assert.strictEqual(util.isError({}), false);
assert.strictEqual(util.isError({ name: 'Error', message: '' }), false);
assert.strictEqual(util.isError([]), false);
assert.strictEqual(util.isError({ __proto__: Error.prototype }), true);

// isObject
assert.strictEqual(util.isObject({}), true);
assert.strictEqual(util.isObject([]), true);
assert.strictEqual(util.isObject(new Number(3)), true);
assert.strictEqual(util.isObject(Number(4)), false);
assert.strictEqual(util.isObject(1), false);

// isPrimitive
assert.strictEqual(util.isPrimitive({}), false);
assert.strictEqual(util.isPrimitive(new Error()), false);
assert.strictEqual(util.isPrimitive(new Date()), false);
assert.strictEqual(util.isPrimitive([]), false);
assert.strictEqual(util.isPrimitive(/regexp/), false);
assert.strictEqual(util.isPrimitive(function() {}), false);
assert.strictEqual(util.isPrimitive(new Number(1)), false);
assert.strictEqual(util.isPrimitive(new String('bla')), false);
assert.strictEqual(util.isPrimitive(new Boolean(true)), false);
assert.strictEqual(util.isPrimitive(1), true);
assert.strictEqual(util.isPrimitive('bla'), true);
assert.strictEqual(util.isPrimitive(true), true);
assert.strictEqual(util.isPrimitive(undefined), true);
assert.strictEqual(util.isPrimitive(null), true);
assert.strictEqual(util.isPrimitive(Infinity), true);
assert.strictEqual(util.isPrimitive(NaN), true);
assert.strictEqual(util.isPrimitive(Symbol('symbol')), true);

assert.strictEqual(util.isFunction(() => {}), true);
assert.strictEqual(util.isFunction(function() {}), true);
assert.strictEqual(util.isFunction(), false);
assert.strictEqual(util.isFunction('string'), false);

assert.strictEqual(util.toUSVString('string\ud801'), 'string\ufffd');

{
  assert.strictEqual(util.types.isNativeError(new Error()), true);
  assert.strictEqual(util.types.isNativeError(new TypeError()), true);
  assert.strictEqual(util.types.isNativeError(new SyntaxError()), true);
  assert.strictEqual(util.types.isNativeError(new (context('Error'))()), true);
  assert.strictEqual(
    util.types.isNativeError(new (context('TypeError'))()),
    true
  );
  assert.strictEqual(
    util.types.isNativeError(new (context('SyntaxError'))()),
    true
  );
  assert.strictEqual(util.types.isNativeError({}), false);
  assert.strictEqual(
    util.types.isNativeError({ name: 'Error', message: '' }),
    false
  );
  assert.strictEqual(util.types.isNativeError([]), false);
  assert.strictEqual(
    util.types.isNativeError({ __proto__: Error.prototype }),
    false
  );
  assert.strictEqual(
    util.types.isNativeError(new errors.codes.ERR_IPC_CHANNEL_CLOSED()),
    true
  );
}

assert.throws(() => {
  util.stripVTControlCharacters({});
}, {
  code: 'ERR_INVALID_ARG_TYPE',
  message: 'The "str" argument must be of type string.' +
           common.invalidArgTypeHelper({})
});
