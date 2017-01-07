'use strict';
const common = require('../common');
const assert = require('assert');
const http = require('http');
const net = require('net');

// RFC 2616, section 10.2.5:
//
//   The 204 response MUST NOT contain a message-body, and thus is always
//   terminated by the first empty line after the header fields.
//
// Likewise for 304 responses. Verify that no empty chunk is sent when
// the user explicitly sets a Transfer-Encoding header.

test(204);
test(304);

function test(statusCode) {
  const server = http.createServer(common.mustCall((req, res) => {
    res.writeHead(statusCode, { 'Transfer-Encoding': 'chunked' });
    res.end();
    server.close();
  }));

  server.listen(0, common.mustCall(() => {
    const conn = net.createConnection(
      server.address().port,
      common.mustCall(() => {
        conn.write('GET / HTTP/1.1\r\n\r\n');

        let resp = '';
        conn.setEncoding('utf8');
        conn.on('data', common.mustCall((data) => {
          resp += data;
        }));

        conn.on('end', common.mustCall(() => {
          // Connection: close should be in the response
          assert.strictEqual(/^Connection: close\r\n$/m.test(resp), true);
          // Make sure this doesn't end with 0\r\n\r\n
          assert.strictEqual(/^0\r\n$/m.test(resp), false);
        }));
      })
    );
  }));
}
