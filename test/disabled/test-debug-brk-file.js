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
const path = require('path');
const net = require('net');

let isDone = false;
const targetPath = path.resolve(common.fixturesDir, 'debug-target.js');

const child = spawn(process.execPath, ['--debug-brk=' + common.PORT, targetPath]);
child.stderr.on('data', function() {
  child.emit('debug_start');
});

child.on('exit', () => {
  assert(isDone);
  console.log('ok');
});

child.once('debug_start', () => {
  // delayed for some time until debug agent is ready
  setTimeout(() => {
    debug_client_connect();
  }, 200);
});


function debug_client_connect() {
  let msg = null;
  let tmpBuf = '';

  const conn = net.connect({port: common.PORT});
  conn.setEncoding('utf8');
  conn.on('data', (data) => {
    tmpBuf += data;
    parse();
  });

  function parse() {
    if (!msg) {
      msg = {
        headers: null,
        contentLength: 0
      };
    }
    if (!msg.headers) {
      const offset = tmpBuf.indexOf('\r\n\r\n');
      if (offset < 0) return;
      msg.headers = tmpBuf.substring(0, offset);
      tmpBuf = tmpBuf.slice(offset + 4);
      const matches = /Content-Length: (\d+)/.exec(msg.headers);
      if (matches[1]) {
        msg.contentLength = +(matches[1]);
      }
    }
    if (msg.headers && Buffer.byteLength(tmpBuf) >= msg.contentLength) {
      try {
        const b = Buffer.from(tmpBuf);
        const body = b.toString('utf8', 0, msg.contentLength);
        tmpBuf = b.toString('utf8', msg.contentLength, b.length);

        // get breakpoint list and check if it exists on line 0
        if (!body.length) {
          var req = JSON.stringify({'seq': 1, 'type': 'request',
                                    'command': 'listbreakpoints'});
          conn.write('Content-Length: ' + req.length + '\r\n\r\n' + req);
          return;
        }

        const obj = JSON.parse(body);
        if (obj.type === 'response' && obj.command === 'listbreakpoints' &&
            !obj.running) {
          obj.body.breakpoints.forEach((bpoint) => {
            if (bpoint.line === 0) isDone = true;
          });
        }

        const req = JSON.stringify({'seq': 100, 'type': 'request',
                                  'command': 'disconnect'});
        conn.write('Content-Length: ' + req.length + '\r\n\r\n' + req);
      } finally {
        msg = null;
        parse();
      }
    }
  }
}
