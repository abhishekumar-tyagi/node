'use strict';
const common = require('../common');
const assert = require('assert');
const cluster = require('cluster');
const net = require('net');

if (common.isWindows) {
  console.log('1..0 # Skipped: not reliable on Windows.');
  return;
}

if (process.getuid() === 0) {
  console.log('1..0 # Skipped: Test is not supposed to be run as root.');
  return;
}

if (cluster.isMaster) {
  cluster.fork().on('exit', common.mustCall(function(exitCode) {
    assert.equal(exitCode, 0);
  }));
}
else {
  var s = net.createServer(assert.fail);
  s.listen(42, assert.fail.bind(null, 'listen should have failed'));
  s.on('error', common.mustCall(function(err) {
    assert.equal(err.code, 'EACCES');
    process.disconnect();
  }));
}
