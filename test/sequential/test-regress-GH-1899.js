'use strict';
const path = require('path');
const assert = require('assert');
const spawn = require('child_process').spawn;
const common = require('../common');

const child = spawn(process.argv[0], [
  path.join(common.fixturesDir, 'GH-1899-output.js')
]);
var output = '';

child.stdout.on('data', function(data) {
  output += data;
});

child.on('exit', function(code, signal) {
  assert.equal(code, 0);
  assert.equal(output, 'hello, world!\n');
});
