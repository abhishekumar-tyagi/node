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
const assert = require('assert');

// Note in Windows one can only set the "user" bits.
let mask;
if (common.isWindows) {
  mask = '0600';
} else {
  mask = '0664';
}

const old = process.umask(mask);

assert.strictEqual(process.umask(old), parseInt(mask, 8));

// confirm reading the umask does not modify it.
// 1. If the test fails, this call will succeed, but the mask will be set to 0
assert.strictEqual(process.umask(), old);
// 2. If the test fails, process.umask() will return 0
assert.strictEqual(process.umask(), old);

assert.throws(() => {
  process.umask({});
}, {
  code: 'ERR_INVALID_ARG_VALUE',
  message: 'The argument \'mask\' must be a 32-bit unsigned integer ' +
           'or an octal string. Received {}'
});

['123x', 'abc', '999'].forEach((value) => {
  assert.throws(() => {
    process.umask(value);
  }, {
    code: 'ERR_INVALID_ARG_VALUE',
    message: 'The argument \'mask\' must be a 32-bit unsigned integer ' +
             `or an octal string. Received '${value}'`
  });
});
