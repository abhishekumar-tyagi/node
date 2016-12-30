'use strict';
const common = require('../common');
const assert = require('assert');

if (!common.hasCrypto) {
  common.skip('missing crypto');
  return;
}
const crypto = require('crypto');

function test() {
  var odd = Buffer.alloc(39, 'A');

  var c = crypto.createDiffieHellman(32);
  c.setPrivateKey(odd);
  c.generateKeys();
}

// FIPS requires a length of at least 1024
if (!common.hasFipsCrypto) {
  assert.doesNotThrow(function() { test(); });
} else {
  assert.throws(function() { test(); }, /key size too small/);
}
