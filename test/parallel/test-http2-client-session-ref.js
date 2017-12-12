'use strict';
// Flags: --expose-internals

const common = require('../common');
const { kSocket } = require('internal/http2/util');
if (!common.hasCrypto)
  common.skip('missing crypto');
const assert = require('assert');
const http2 = require('http2');
const { PassThrough } = require('stream');

function isActiveHandle(h) {
  return process._getActiveHandles().indexOf(h) !== -1;
}

const server = http2.createServer();
server.listen(0, common.mustCall(() => {
  const port = server.address().port;

  let client = http2.connect(`http://localhost:${port}`);
  assert.ok(isActiveHandle(client[kSocket]));
  client.unref();
  assert.ok(!isActiveHandle(client[kSocket]));
  client.ref();
  assert.ok(isActiveHandle(client[kSocket]));
  client.destroy();
  assert.ok(!isActiveHandle(client[kSocket]));
  // Ensure that calling these methods don't throw after session is destroyed.
  assert.doesNotThrow(client.unref.bind(client));
  assert.doesNotThrow(client.ref.bind(client));
  assert.ok(!isActiveHandle(client[kSocket]));

  server.close();

  // Try with generic DuplexStream
  client = http2.connect(`http://localhost:${port}`, {
    createConnection: common.mustCall(() => PassThrough())
  });
  assert.doesNotThrow(client.unref.bind(client));
  assert.doesNotThrow(client.ref.bind(client));
  client.destroy();
  assert.doesNotThrow(client.unref.bind(client));
  assert.doesNotThrow(client.ref.bind(client));
}));
