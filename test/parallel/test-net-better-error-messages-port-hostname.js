'use strict';
var common = require('../common');
var net = require('net');
var assert = require('assert');

var c = net.createConnection(common.PORT, 'blah.blah');

c.on('connect', assert.fail);

c.on('error', common.mustCall(function(e) {
  assert.equal(e.code, 'ENOTFOUND');
  assert.equal(e.port, common.PORT);
  assert.equal(e.hostname, 'blah.blah');
}));
