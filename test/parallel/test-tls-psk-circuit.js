'use strict';
const common = require('../common');

if (!common.hasCrypto)
  common.skip('missing crypto');

const assert = require('assert');
const tls = require('tls');

const CIPHERS = 'PSK+HIGH:TLS_AES_128_GCM_SHA256';
const USERS = {
  UserA: Buffer.allocUnsafe(128),
  UserB: Buffer.from('82072606b502b0f4025e90eb75fe137d', 'hex'),
};
const TEST_DATA = 'x';

const serverOptions = {
  ciphers: CIPHERS,
  pskCallback(socket, id) {
    assert.ok(socket instanceof tls.TLSSocket);
    assert.ok(typeof id === 'string');
    return USERS[id];
  },
};

function test(secret, opts, error) {
  const cb = !error ?
    common.mustCall((c) => { c.pipe(c); }) :
    common.mustNotCall();
  const server = tls.createServer(serverOptions, cb);
  server.listen(0, common.mustCall(() => {
    const options = {
      port: server.address().port,
      ciphers: CIPHERS,
      checkServerIdentity: () => {},
      pskCallback: common.mustCall(() => secret),
      ...opts,
    };

    if (!error) {
      const client = tls.connect(options, common.mustCall(() => {
        client.end(TEST_DATA);

        client.on('data', common.mustCall((data) => {
          assert.strictEqual(data.toString(), TEST_DATA);
        }));
        client.on('close', common.mustCall(() => server.close()));
      }));
    } else {
      const client = tls.connect(options, common.mustNotCall());
      client.on('error', common.mustCall((err) => {
        assert.strictEqual(err.message, error);
        server.close();
      }));
    }
  }));
}

const DISCONNECT_MESSAGE =
  'Client network socket disconnected before ' +
  'secure TLS connection was established';

test({ psk: USERS.UserA, identity: 'UserA' });
test({ psk: USERS.UserA, identity: 'UserA' }, { maxVersion: 'TLSv1.2' });
test({ psk: USERS.UserA, identity: 'UserA' }, { minVersion: 'TLSv1.3' });
test({ psk: USERS.UserB, identity: 'UserB' });
test({ psk: USERS.UserB, identity: 'UserB' }, { minVersion: 'TLSv1.3' });
// Unrecognized user should fail handshake
test({ psk: USERS.UserB, identity: 'UserC' }, {}, DISCONNECT_MESSAGE);
// Recognized user but incorrect secret should fail handshake
test({ psk: USERS.UserA, identity: 'UserB' }, {}, DISCONNECT_MESSAGE);
test({ psk: USERS.UserB, identity: 'UserB' });
