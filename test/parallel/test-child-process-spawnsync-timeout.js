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

const spawnSync = require('child_process').spawnSync;

const TIMER = 200;
const SLEEP = common.platformTimeout(5000);

switch (process.argv[2]) {
  case 'child':
    setTimeout(function() {
      console.log('child fired');
      process.exit(1);
    }, SLEEP);
    break;
  default:
    const start = Date.now();
    const ret = spawnSync(process.execPath, [__filename, 'child'],
                        { timeout: TIMER });
    assert.strictEqual(ret.error.errno, 'ETIMEDOUT');
    const end = Date.now() - start;
    assert(end < SLEEP);
    assert(ret.status > 128 || ret.signal);
    break;
}
