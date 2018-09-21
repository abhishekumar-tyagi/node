'use strict';

const common = require('../common');

if (!common.hasCrypto)
  common.skip('missing crypto');

const h2 = require('http2');
const net = require('net');

const preamble = Buffer.from([
  0x50, 0x52, 0x49, 0x20, 0x2a, 0x20, 0x48, 0x54, 0x54, 0x50, 0x2f,
  0x32, 0x2e, 0x30, 0x0d, 0x0a, 0x0d, 0x0a, 0x53, 0x4d, 0x0d, 0x0a,
  0x0d, 0x0a, 0x00, 0x00, 0x0c, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x03, 0x00, 0x00, 0x00, 0x64, 0x00, 0x04, 0x00, 0x00, 0xff,
  0xff, 0x00, 0x00, 0x00, 0x04, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x05, 0x02, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
  0x00, 0xc8, 0x00, 0x00, 0x05, 0x02, 0x00, 0x00, 0x00, 0x00, 0x05,
  0x00, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x05, 0x02, 0x00, 0x00,
  0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05,
  0x02, 0x00, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x07, 0x00,
  0x00, 0x00, 0x05, 0x02, 0x00, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x00,
  0x00, 0x03, 0x00, 0x00, 0x00, 0x2c, 0x01, 0x24, 0x00, 0x00, 0x00,
  0x0d, 0x00, 0x00, 0x00, 0x0b, 0x0f, 0x83, 0x84, 0x86, 0x41, 0x8a,
  0xa0, 0xe4, 0x1d, 0x13, 0x9d, 0x09, 0xb8, 0xf0, 0x1e, 0x07, 0x53,
  0x03, 0x2a, 0x2f, 0x2a, 0x90, 0x7a, 0x8a, 0xaa, 0x69, 0xd2, 0x9a,
  0xc4, 0xc0, 0x57, 0x0b, 0xcb, 0x87, 0x0f, 0x0d, 0x83, 0x08, 0x00,
  0x0f
]);

const data = Buffer.from([
  0x00, 0x00, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0d,
  0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x0a, 0x68, 0x65, 0x6c,
  0x6c, 0x6f, 0x0a, 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x0a
]);

// This is testing the case of a misbehaving client that is not paying
// attention to flow control. The initial window size is set to data
// payload * 2, which in this case is 36, the stream is paused so
// WINDOW_UPDATE frames are not being sent, which means the window
// size is not being updated. A well behaved client is supposed to
// stop sending until the window size is expanded again.
//
// However, our malicious client keeps sending data beyond the flow
// control window!
//
// Bad client! Bad!
//
// Fortunately, nghttp2 handles this situation for us by keeping track
// of the flow control window and responding with a FLOW_CONTROL_ERROR
// causing the stream to get shut down...
//
// At least, that's what is supposed to happen.

let client;

const server = h2.createServer({ settings: { initialWindowSize: 36 } });
server.on('stream', (stream) => {
  // Set the high water mark to zero, since otherwise we still accept
  // reads from the source stream (if we can consume them).
  stream._readableState.highWaterMark = 0;
  stream.pause();
  stream.on('error', common.expectsError({
    code: 'ERR_HTTP2_STREAM_ERROR',
    type: Error,
    message: 'Stream closed with error code NGHTTP2_FLOW_CONTROL_ERROR'
  }));
  stream.on('close', common.mustCall(() => {
    server.close();
    client.destroy();
  }));
  stream.on('end', common.mustNotCall());
});

server.listen(0, () => {
  client = net.connect(server.address().port, () => {
    client.write(preamble);
    client.write(data);
    client.write(data);
    client.write(data);
  });
});
