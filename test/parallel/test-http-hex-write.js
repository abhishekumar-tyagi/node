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

const expect = 'hex\nutf8\n';

const server = http.createServer(function(q, s) {
  s.setHeader('content-length', expect.length);
  s.write('6865780a', 'hex');
  s.write('utf8\n');
  s.end();
}).listen(0, common.mustCall(function() {
  http.request({ port: this.address().port })
    .on('response', common.mustCall(function(res) {
      let data = '';

      res.setEncoding('ascii');
      res.on('data', function(c) {
        data += c;
      });
      res.on('end', common.mustCall(function() {
        assert.strictEqual(data, expect);
      }));
    })).end();

  // Refs: https://github.com/nodejs/node/issues/45150
  const invalidReq = http.request({ port: this.address().port });
  assert.throws(
    () => { invalidReq.write('boom!', 'hex'); },
    {
      code: 'ERR_INVALID_ARG_VALUE',
      message: 'The argument \'data\' is invalid hex string. Received \'boom!\''
    }
  );
  invalidReq.end(() => server.close());
}));
