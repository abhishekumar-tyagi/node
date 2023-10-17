'use strict';
const common = require('../common');
if (!common.hasCrypto)
  common.skip('missing crypto');

const assert = require('assert');
const tls = require('tls');
const net = require('net');
const Countdown = require('../common/countdown');
const fixtures = require('../common/fixtures');

const key = fixtures.readKey('agent2-key.pem');
const cert = fixtures.readKey('agent2-cert.pem');

let serverTlsSocket;
const tlsServer = tls.createServer({ cert, key }, (socket) => {
  serverTlsSocket = socket;
  socket.on('close', dec);
});

// A plain net server, that manually passes connections to the TLS
// server to be upgraded.
let netSocket;
let netSocketCloseEmitted = false;
const netServer = net.createServer((socket) => {
  netSocket = socket;
  tlsServer.emit('connection', socket);
  socket.on('close', () => {
    netSocketCloseEmitted = true;
    assert.strictEqual(serverTlsSocket.destroyed, true);
  });
}).listen(0, common.mustCall(() => {
  connectClient(netServer);
}));

const countdown = new Countdown(2, () => {
  netServer.close();
});

// A client that connects, sends one message, and closes the raw connection:
function connectClient(server) {
  const clientTlsSocket = tls.connect({
    host: 'localhost',
    port: server.address().port,
    rejectUnauthorized: false
  });

  clientTlsSocket.write('foo', 'utf8', common.mustCall(() => {
    assert(netSocket);
    netSocket.setTimeout(common.platformTimeout(10), common.mustCall(() => {
      assert(serverTlsSocket);

      netSocket.destroy();
      assert.strictEqual(netSocket.destroyed, true);

      setImmediate(() => {
        // Close callbacks are executed after `setImmediate()` callbacks.
        assert.strictEqual(netSocketCloseEmitted, false);
        assert.strictEqual(serverTlsSocket.destroyed, false);
        setImmediate(() => {
          assert.strictEqual(netSocketCloseEmitted, true);
        });
      });
    }));
  }));

  clientTlsSocket.on('close', dec);
}

function dec() {
  countdown.dec();
}
