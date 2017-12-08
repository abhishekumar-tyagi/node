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
const Countdown = require('../common/countdown');
const assert = require('assert');
const http = require('http');

const countdown = new Countdown(2, common.mustCall(() => {
  server.close();
}));

const server = http.createServer(function(req, res) {
  if (req.url === '/one') {
    res.writeHead(200, [['set-cookie', 'A'],
                        ['content-type', 'text/plain']]);
    res.end('one\n');
  } else {
    res.writeHead(200, [['set-cookie', 'A'],
                        ['set-cookie', 'B'],
                        ['content-type', 'text/plain']]);
    res.end('two\n');
  }
});
server.listen(0);

server.on('listening', function() {
  //
  // one set-cookie header
  //
  http.get({ port: this.address().port, path: '/one' }, function(res) {
    // set-cookie headers are always return in an array.
    // even if there is only one.
    assert.deepStrictEqual(['A'], res.headers['set-cookie']);
    assert.strictEqual('text/plain', res.headers['content-type']);

    res.on('data', function(chunk) {
      console.log(chunk.toString());
    });

    res.on('end', function() {
      countdown.dec();
    });
  });

  // two set-cookie headers

  http.get({ port: this.address().port, path: '/two' }, function(res) {
    assert.deepStrictEqual(['A', 'B'], res.headers['set-cookie']);
    assert.strictEqual('text/plain', res.headers['content-type']);

    res.on('data', function(chunk) {
      console.log(chunk.toString());
    });

    res.on('end', function() {
      countdown.dec();
    });
  });

});
