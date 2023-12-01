'use strict';
const common = require('../common');
const dnstools = require('../common/dns');
const dns = require('dns');
const assert = require('assert');
const dgram = require('dgram');
const dnsPromises = dns.promises;

const server = dgram.createSocket('udp4');
const rdns = new dns.Resolver({ timeout: 3000, retry: 1 });
const rdnsPromises = new dnsPromises.Resolver({ timeout: 3000, retry: 1 });

server.on('message', common.mustCall((msg, { address, port }) => {
  const parsed = dnstools.parseDNSPacket(msg);
  const domain = parsed.questions[0].domain;
  assert.strictEqual(domain, 'example.org');

  const buf = dnstools.writeDNSPacket({
    id: parsed.id,
    questions: parsed.questions,
    answers: { type: 'A', address: '1.2.3.4', ttl: 123, domain },
  });
  // Overwrite the # of answers with 2, which is incorrect.
  buf.writeUInt16LE(2, 6);
  server.send(buf, port, address);
}, 2));

server.bind(0, common.mustCall(async () => {
  const address = server.address();
  rdns.setServers([`127.0.0.1:${address.port}`]);
  rdnsPromises.setServers([`127.0.0.1:${address.port}`]);

  rdnsPromises.resolveAny('example.org')
    .then(common.mustNotCall())
    .catch(common.expectsError({
      // May return EBADRESP or ETIMEOUT
      code: /^(?:EBADRESP|ETIMEOUT)$/,
      syscall: 'queryAny',
      hostname: 'example.org'
    }));

  rdns.resolveAny('example.org', common.mustCall((err) => {
    assert.notStrictEqual(err.code, 'SUCCESS');
    assert.strictEqual(err.syscall, 'queryAny');
    assert.strictEqual(err.hostname, 'example.org');
    const descriptor = Object.getOwnPropertyDescriptor(err, 'message');
    // The error message should be non-enumerable.
    assert.strictEqual(descriptor.enumerable, false);
    server.close();
  }));
}));
