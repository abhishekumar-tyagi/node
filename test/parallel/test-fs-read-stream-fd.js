'use strict';
const common = require('../common');
const fs = require('fs');
const assert = require('assert');
const path = require('path');
var file = path.join(common.tmpDir, '/read_stream_fd_test.txt');
var input = 'hello world';
var output = '';
var fd, stream;

common.refreshTmpDir();
fs.writeFileSync(file, input);
fd = fs.openSync(file, 'r');

stream = fs.createReadStream(null, { fd: fd, encoding: 'utf8' });
stream.on('data', function(data) {
  output += data;
});

process.on('exit', function() {
  fs.unlinkSync(file);
  assert.equal(output, input);
});
