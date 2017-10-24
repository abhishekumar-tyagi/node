'use strict';

const common = require('../common');
if (!common.hasCrypto)
common.skip('missing crypto');
const assert = require('assert');
const http2 = require('http2');

const expectedWeight = 256;

const server = http2.createServer();
server.on('stream', common.mustCall((stream, headers, flags) => {
  const options = { weight: expectedWeight };
  stream.pushStream({}, options, common.mustCall((stream, headers, flags) => {
    // The priority of push streams is changed silently by nghttp2,
    // without emitting a PRIORITY frame. Flags are ignored.
    assert.strictEqual(stream.state.weight, expectedWeight);
    // assert.ok(flags & http2.constants.NGHTTP2_FLAG_PRIORITY);
  }));
  stream.respond({
    'content-type': 'text/html',
    ':status': 200
  });
  stream.end('test');
}));

server.listen(0, common.mustCall(() => {
  const port = server.address().port;
  const client = http2.connect(`http://localhost:${port}`);

  const headers = { ':path': '/' };
  const req = client.request(headers);

  client.on('stream', common.mustCall((stream, headers, flags) => {
    // Since the push priority is set silently, the client is not informed.
    // assert.strictEqual(stream.state.weight, expectedWeight);
    // assert.ok(flags & http2.constants.NGHTTP2_FLAG_PRIORITY);
  }));

  req.on('data', common.mustCall(() => {}));
  req.on('end', common.mustCall(() => {
    server.close();
    client.destroy();
  }));
  req.end();
}));
