var common = require('../common');
var assert = require('assert');
var dgram = require('dgram');

var message = new Buffer('Some bytes');
var client = dgram.createSocket('udp4');
client.send(message, 0, message.length, 41234, "localhost", function(err, bytes) {
  assert.strictEqual(bytes, message.length);
  client.close();
});
