'use strict';
const common = require('../common');
const assert = require('assert');
const fork = require('child_process').fork;

var expected = Array(1e5).join('ßßßß');
if (process.argv[2] === 'child') {
  process.send(expected);
} else {
  var child = fork(process.argv[1], ['child']);
  child.on('message', common.mustCall(function(actual) {
    assert.equal(actual, expected);
  }));
}
