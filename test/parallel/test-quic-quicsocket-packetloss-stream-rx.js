// Flags: --no-warnings
'use strict';

// Tests that stream data is successfully transmitted under
// packet loss conditions on the receiving end.

// TODO(@jasnell): We need an equivalent test that checks
// transmission end random packet loss.

const common = require('../common');
if (!common.hasQuic)
  common.skip('missing quic');

common.skip('temporarily skip packetloss tests for refactoring');

const Countdown = require('../common/countdown');
const assert = require('assert');
const {
  key,
  cert,
  ca,
  debug
} = require('../common/quic');
// TODO(@jasnell): There's currently a bug in pipeline when piping
// a duplex back into to itself.
// const { pipeline } = require('stream');

const { createQuicSocket } = require('net');

const kData = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ';
const options = { key, cert, ca, alpn: 'echo' };

const client = createQuicSocket({ client: options });
const server = createQuicSocket({ server: options });

// Both client and server will drop received packets about 20% of the time
// It is important to keep in mind that this will make the runtime of the
// test non-deterministic. If we encounter flaky timeouts with this test,
// the randomized packet loss will be the reason, but random packet loss
// is exactly what is being tested. So if flaky timeouts do occur, it will
// be best to extend the failure timeout for this test.
server.setDiagnosticPacketLoss({ rx: 0.2 });
client.setDiagnosticPacketLoss({ rx: 0.2 });

const countdown = new Countdown(1, () => {
  debug('Countdown expired. Destroying sockets');
  server.close();
  client.close();
});

server.listen();
server.on('session', common.mustCall((session) => {
  debug('QuicServerSession Created');

  session.on('stream', common.mustCall((stream) => {
    debug('Bidirectional, Client-initiated stream %d received', stream.id);
    stream.on('data', (chunk) => stream.write(chunk));
    stream.on('end', () => stream.end());
    // TODO(@jasnell): There's currently a bug in pipeline when piping
    // a duplex back into to itself.
    // pipeline(stream, stream, common.mustCall((err) => {
    //   assert(!err);
    // }));
  }));

}));

server.on('ready', common.mustCall(() => {
  debug('Server is listening on port %d', server.endpoints[0].address.port);

  const req = client.connect({
    address: common.localhostIPv4,
    port: server.endpoints[0].address.port,
  });

  req.on('secure', common.mustCall((servername, alpn, cipher) => {
    debug('QuicClientSession TLS Handshake Complete');

    const stream = req.openStream();

    let n = 0;
    // This forces multiple stream packets to be sent out
    // rather than all the data being written in a single
    // packet.
    function sendChunk() {
      if (n < kData.length) {
        stream.write(kData[n++], common.mustCall());
        setImmediate(sendChunk);
      } else {
        stream.end();
      }
    }
    sendChunk();

    let data = '';
    stream.resume();
    stream.setEncoding('utf8');
    stream.on('data', (chunk) => data += chunk);
    stream.on('end', common.mustCall(() => {
      debug('Received data: %s', kData);
      assert.strictEqual(data, kData);
    }));

    stream.on('close', common.mustCall(() => {
      debug('Bidirectional, Client-initiated stream %d closed', stream.id);
      countdown.dec();
    }));

    debug('Bidirectional, Client-initiated stream %d opened', stream.id);
  }));
}));
