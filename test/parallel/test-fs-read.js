'use strict';
const common = require('../common');
const assert = require('assert');
const path = require('path');
const fs = require('fs');
var filepath = path.join(common.fixturesDir, 'x.txt'),
    fd = fs.openSync(filepath, 'r'),
    expected = 'xyz\n',
    readCalled = 0;

fs.read(fd, expected.length, 0, 'utf-8', function(err, str, bytesRead) {
  readCalled++;

  assert.ok(!err);
  assert.equal(str, expected);
  assert.equal(bytesRead, expected.length);
});

var r = fs.readSync(fd, expected.length, 0, 'utf-8');
assert.equal(r[0], expected);
assert.equal(r[1], expected.length);

process.on('exit', function() {
  assert.equal(readCalled, 1);
});
