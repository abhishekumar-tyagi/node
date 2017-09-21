'use strict';

// Tests the basic operation of creating a plaintext or TLS
// HTTP2 server. The server does not do anything at this point
// other than start listening.

const common = require('../common');

if (!common.hasCrypto)
  common.skip('missing crypto');

const assert = require('assert');
const http2 = require('http2');
const path = require('path');
const tls = require('tls');
const net = require('net');
const fs = require('fs');

const options = {
  key: fs.readFileSync(
    path.resolve(common.fixturesDir, 'keys/agent2-key.pem')),
  cert: fs.readFileSync(
    path.resolve(common.fixturesDir, 'keys/agent2-cert.pem'))
};

// There should not be any throws
assert.doesNotThrow(() => {

  const serverTLS = http2.createSecureServer(options, () => {});

  serverTLS.listen(0, common.mustCall(() => serverTLS.close()));

  // There should not be an error event reported either
  serverTLS.on('error', common.mustNotCall());
});

// There should not be any throws
assert.doesNotThrow(() => {
  const server = http2.createServer(options, common.mustNotCall());

  server.listen(0, common.mustCall(() => server.close()));

  // There should not be an error event reported either
  server.on('error', common.mustNotCall());
});

// Test the plaintext server socket timeout
{
  let client;
  const server = http2.createServer();
  server.on('timeout', common.mustCall(() => {
    server.close();
    if (client)
      client.end();
  }));
  server.setTimeout(common.platformTimeout(1000));
  server.listen(0, common.mustCall(() => {
    const port = server.address().port;
    client = net.connect(port, common.mustCall());
  }));
}

// Test the secure server socket timeout
{
  let client;
  const server = http2.createSecureServer(options);
  server.on('timeout', common.mustCall(() => {
    server.close();
    if (client)
      client.end();
  }));
  server.setTimeout(common.platformTimeout(1000));
  server.listen(0, common.mustCall(() => {
    const port = server.address().port;
    client = tls.connect({
      port: port,
      rejectUnauthorized: false,
      ALPNProtocols: ['h2']
    }, common.mustCall());
  }));
}
