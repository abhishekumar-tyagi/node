'use strict';
const common = require('../common');
const assert = require('assert');
const http = require('http');
const MakeDuplexPair = require('../common/duplexpair');

// Test that setting the `maxHeaderSize` option works on a per-stream-basis.

// Test 1: The server sends an invalid header.
{
  const { clientSide, serverSide } = MakeDuplexPair();

  const req = http.request({
    createConnection: common.mustCall(() => clientSide),
    insecureHTTPParser: true
  }, common.mustCall((res) => {
    assert.strictEqual(res.headers.hello, 'foo\x08foo');
    res.resume();  // We don’t actually care about contents.
    res.on('end', common.mustCall());
  }));
  req.end();

  serverSide.resume();  // Dump the request
  serverSide.end('HTTP/1.1 200 OK\r\n' +
                 'Host: example.com\r\n' +
                 'Hello: foo\x08foo\r\n' +
                 'Content-Length: 0\r\n' +
                 '\r\n\r\n');
}

// Test 2: The same as Test 1 except without the option, to make sure it fails.
{
  const { clientSide, serverSide } = MakeDuplexPair();

  const req = http.request({
    createConnection: common.mustCall(() => clientSide)
  }, common.mustNotCall());
  req.end();
  req.on('error', common.mustCall());

  serverSide.resume();  // Dump the request
  serverSide.end('HTTP/1.1 200 OK\r\n' +
                 'Host: example.com\r\n' +
                 'Hello: foo\x08foo\r\n' +
                 'Content-Length: 0\r\n' +
                 '\r\n\r\n');
}

// Test 3: The client sends an invalid header.
{
  const testData = 'Hello, World!\n';
  const server = http.createServer(
    { insecureHTTPParser: true },
    common.mustCall((req, res) => {
      res.statusCode = 200;
      res.setHeader('Content-Type', 'text/plain');
      res.end(testData);
    }));

  server.on('clientError', common.mustNotCall());

  const { clientSide, serverSide } = MakeDuplexPair();
  serverSide.server = server;
  server.emit('connection', serverSide);

  clientSide.write('GET / HTTP/1.1\r\n' +
                   'Host: example.com\r\n' +
                   'Hello: foo\x08foo\r\n' +
                   '\r\n\r\n');
}

// Test 4: The same as Test 3 except without the option, to make sure it fails.
{
  const server = http.createServer(common.mustNotCall());

  server.on('clientError', common.mustCall());

  const { clientSide, serverSide } = MakeDuplexPair();
  serverSide.server = server;
  server.emit('connection', serverSide);

  clientSide.write('GET / HTTP/1.1\r\n' +
                   'Host: example.com\r\n' +
                   'Hello: foo\x08foo\r\n' +
                   '\r\n\r\n');
}

// Test 5: Invalid argument type
{
  assert.throws(
    () => http.request({ insecureHTTPParser: 0 }, common.mustNotCall()),
    common.expectsError({
      code: 'ERR_INVALID_ARG_TYPE',
      message: 'The "options.insecureHTTPParser" property must be of' +
      ' type boolean. Received type number (0)'
    })
  );
}
