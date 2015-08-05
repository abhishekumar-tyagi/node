'use strict';
const common = require('../common');
const assert = require('assert');
const domain = require('domain');

if (!common.hasCrypto) {
  console.log('1..0 # Skipped: missing crypto');
  return;
}
const crypto = require('crypto');

function test(fn) {
  var ex = new Error('BAM');
  var d = domain.create();
  d.on('error', common.mustCall(function(err) {
    assert.equal(err, ex);
  }));
  var cb = common.mustCall(function() {
    throw ex;
  });
  d.run(cb);
}

test(function(cb) {
  crypto.pbkdf2('password', 'salt', 1, 8, cb);
});

test(function(cb) {
  crypto.randomBytes(32, cb);
});
