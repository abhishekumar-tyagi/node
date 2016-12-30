'use strict';
const common = require('../common');
const assert = require('assert');

if (!common.hasFipsCrypto) {
  common.skip('node compiled without FIPS OpenSSL.');
  return;
}

const crypto = require('crypto');
const fs = require('fs');

var input = 'hello';

var dsapri = fs.readFileSync(common.fixturesDir +
                             '/keys/dsa_private_1025.pem');
var sign = crypto.createSign('DSS1');
sign.update(input);

assert.throws(function() {
  sign.sign(dsapri);
}, /PEM_read_bio_PrivateKey failed/);
