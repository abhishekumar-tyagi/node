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
require('../common');
const assert = require('assert');
const { MessageChannel } = require('worker_threads');

// setImmediate should run clear its queued cbs once per event loop turn
// but immediates queued while processing the current queue should happen
// on the next turn of the event loop.

// hit should be the exact same size of QUEUE, if we're letting things
// recursively add to the immediate QUEUE hit will be > QUEUE

// We use MessagePorts to figure out whether the event loop has progressed
// rather than timers because they are not subject to timer-precision-related
// flakiness, see https://github.com/nodejs/node/issues/24497 for details.
let ticked = false;
{
  const { port1, port2 } = new MessageChannel();
  port1.onmessage = () => ticked = true;
  port1.unref();
  port2.postMessage('');
}

let hit = 0;
const QUEUE = 10;

function run() {
  if (ticked) return;

  hit += 1;
  setImmediate(run);
}

for (let i = 0; i < QUEUE; i++)
  setImmediate(run);

process.on('exit', function() {
  console.log('hit', hit);
  assert.strictEqual(hit, QUEUE);
});
