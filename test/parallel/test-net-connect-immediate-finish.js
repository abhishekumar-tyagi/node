'use strict';
const common = require('../common');
const assert = require('assert');
const net = require('net');

var gotError = false;

var client = net.connect({
  host: 'no.way.you.will.resolve.this',
  port: common.PORT
});

client.once('error', function(err) {
  gotError = true;
});

client.end();

process.on('exit', function() {
  assert(gotError);
});
