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
const net = require('net');
const assert = require('assert');

// Verify that invalid delays throw
const s = new net.Socket();
const nonNumericDelays = [
  '100', true, false, undefined, null, '', {}, common.noop, []
];
const badRangeDelays = [-0.001, -1, -Infinity, Infinity, NaN];
const validDelays = [0, 0.001, 1, 1e6];


for (let i = 0; i < nonNumericDelays.length; i++) {
<<<<<<< HEAD
  assert.throws(function() {
<<<<<<< HEAD
    s.setTimeout(nonNumericDelays[i], common.noop);
  }, TypeError);
=======
=======
  assert.throws(() => {
>>>>>>> test: refactor functions to use arrow functions
    s.setTimeout(nonNumericDelays[i], noop);
  }, common.expectsError('ERR_INVALID_ARG_TYPE', TypeError));
>>>>>>> timers, test: migrate to internal/errors.js
}

for (let i = 0; i < badRangeDelays.length; i++) {
<<<<<<< HEAD
  assert.throws(function() {
<<<<<<< HEAD
    s.setTimeout(badRangeDelays[i], common.noop);
  }, RangeError);
=======
=======
  assert.throws(() => {
>>>>>>> test: refactor functions to use arrow functions
    s.setTimeout(badRangeDelays[i], noop);
  }, common.expectsError('ERR_INVALID_ARG_TYPE', RangeError));
>>>>>>> timers, test: migrate to internal/errors.js
}

for (let i = 0; i < validDelays.length; i++) {
<<<<<<< HEAD
  assert.doesNotThrow(function() {
    s.setTimeout(validDelays[i], common.noop);
=======
  assert.doesNotThrow(() => {
    s.setTimeout(validDelays[i], noop);
>>>>>>> test: refactor functions to use arrow functions
  });
}

const server = net.Server();
server.listen(0, common.mustCall(() => {
  const socket = net.createConnection(server.address().port);
  socket.setTimeout(1, common.mustCall(() => {
    socket.destroy();
    server.close();
  }));
}));
