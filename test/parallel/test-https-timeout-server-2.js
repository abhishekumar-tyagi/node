'use strict';

const common = require('../common');
const assert = require('assert');

if (!common.hasCrypto) {
  console.log('1..0 # Skipped: missing crypto');
  return;
}
const https = require('https');

const net = require('net');
const tls = require('tls');
const fs = require('fs');

var options = {
  key: fs.readFileSync(common.fixturesDir + '/keys/agent1-key.pem'),
  cert: fs.readFileSync(common.fixturesDir + '/keys/agent1-cert.pem')
};

var server = https.createServer(options, assert.fail);

server.on('secureConnection', function(cleartext) {
  var s = cleartext.setTimeout(50, function() {
    cleartext.destroy();
    server.close();
  });
  assert.ok(s instanceof tls.TLSSocket);
});

server.listen(common.PORT, function() {
  tls.connect({
    host: '127.0.0.1',
    port: common.PORT,
    rejectUnauthorized: false
  });
});
