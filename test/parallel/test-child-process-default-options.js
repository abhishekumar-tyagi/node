'use strict';
var common = require('../common');
var assert = require('assert');

var spawn = require('child_process').spawn;

var isWindows = common.isWindows;

process.env.HELLO = 'WORLD';

if (isWindows) {
  var child = spawn('cmd.exe', ['/c', 'set'], {});
} else {
  var child = spawn('/usr/bin/env', [], {});
}

var response = '';

child.stdout.setEncoding('utf8');

child.stdout.on('data', function(chunk) {
  console.log('stdout: ' + chunk);
  response += chunk;
});

process.on('exit', function() {
  assert.ok(response.indexOf('HELLO=WORLD') >= 0,
            'spawn did not use process.env as default');
});
