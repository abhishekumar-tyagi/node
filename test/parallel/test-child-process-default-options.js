// Copyright Joyent, Inc. and other Node contributors.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so, subject to the
// following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
// NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
// USE OR OTHER DEALINGS IN THE SOFTWARE.

'use strict';
const common = require('../common');
const assert = require('assert');

const spawn = require('child_process').spawn;

process.env.HELLO = 'WORLD';

let child;
if (common.isWindows) {
  child = spawn('cmd.exe', ['/c', 'set'], {});
} else {
  child = spawn('/usr/bin/env', [], {});
}

let response = '';

child.stdout.setEncoding('utf8');

child.stdout.on('data', function(chunk) {
  console.log(`stdout: ${chunk}`);
  response += chunk;
});

process.on('exit', function() {
  assert.ok(response.includes('HELLO=WORLD'),
            'spawn did not use process.env as default' +
            `(process.env.HELLO = ${process.env.HELLO})`);
});
