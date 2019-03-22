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
const http = require('http');

const server = http.createServer();

server.once('request', common.mustCall((req, res) => {
  server.on('request', common.mustCall((req, res) => {
    res.end(Buffer.from('asdf'));
  }));
  // Write should accept string
  res.write('string');
  // Write should accept buffer
  res.write(Buffer.from('asdf'));

  const expectedError = {
    code: 'ERR_INVALID_ARG_TYPE',
    name: 'TypeError',
  };

  // Write should not accept an Array
  assert.throws(
    () => {
      res.write(['array']);
    },
    expectedError
  );

  // End should not accept an Array
  assert.throws(
    () => {
      res.end(['moo']);
    },
    expectedError
  );

  // End should accept string
  res.end('string');
}));

server.listen(0, function() {
  // Just make a request, other tests handle responses
  http.get({ port: this.address().port }, (res) => {
    res.resume();
    // Do it again to test .end(Buffer);
    http.get({ port: server.address().port }, (res) => {
      res.resume();
      server.close();
    });
  });
});
