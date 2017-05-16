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

// This test ensures that `net.connect` with an unfindable host will emit
// an error, and we assert the structure of the error.

const assert = require('assert');
const net = require('net');

// since this test should fail with `ENOTFOUND` becuase of the unfindable host
// we don't care about the port, so we just pick an arbitrary but valid port
const arbitrary_port = common.PORT
const unfindable_host = '***';
const client = net.connect({host: unfindable_host, port: arbitrary_port});

client.once('error', common.mustCall((err) => {
  assert.ok(err);
  assert.strictEqual(err.code, err.errno);
  assert.strictEqual(err.code, 'ENOTFOUND');
  assert.strictEqual(err.host, err.hostname);
  assert.strictEqual(err.host, unfindable_host);
  assert.strictEqual(err.syscall, 'getaddrinfo');
  assert.strictEqual(
    err.message,
    `getaddrinfo ENOTFOUND ${unfindable_host} ${unfindable_host}:${err.port}`
  );
  client.end();
}));
