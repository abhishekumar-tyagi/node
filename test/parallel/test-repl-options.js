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
const repl = require('repl');

common.globalCheck = false;

// Create a dummy stream that does nothing
const stream = new common.ArrayStream();

// 1, mostly defaults
const r1 = repl.start({
  input: stream,
  output: stream,
  terminal: true
});

assert.strictEqual(r1.input, stream);
assert.strictEqual(r1.output, stream);
assert.strictEqual(r1.input, r1.inputStream);
assert.strictEqual(r1.output, r1.outputStream);
assert.strictEqual(r1.terminal, true);
assert.strictEqual(r1.useColors, r1.terminal);
assert.strictEqual(r1.useGlobal, false);
assert.strictEqual(r1.ignoreUndefined, false);
assert.strictEqual(r1.replMode, repl.REPL_MODE_SLOPPY);
assert.strictEqual(r1.historySize, 30);
assert.strictEqual(r1.displayWelcomeMessage, false);
assert.strictEqual(r1.executeOnTimeout, 50);

// test r1 for backwards compact
assert.strictEqual(r1.rli.input, stream);
assert.strictEqual(r1.rli.output, stream);
assert.strictEqual(r1.rli.input, r1.inputStream);
assert.strictEqual(r1.rli.output, r1.outputStream);
assert.strictEqual(r1.rli.terminal, true);
assert.strictEqual(r1.useColors, r1.rli.terminal);

// 2
function writer() {}
function evaler() {}
const r2 = repl.start({
  input: stream,
  output: stream,
  terminal: false,
  useColors: true,
  useGlobal: true,
  ignoreUndefined: true,
  eval: evaler,
  writer: writer,
  replMode: repl.REPL_MODE_STRICT,
  historySize: 50,
  displayWelcomeMessage: true,
  executeOnTimeout: 500
});
assert.strictEqual(r2.input, stream);
assert.strictEqual(r2.output, stream);
assert.strictEqual(r2.input, r2.inputStream);
assert.strictEqual(r2.output, r2.outputStream);
assert.strictEqual(r2.terminal, false);
assert.strictEqual(r2.useColors, true);
assert.strictEqual(r2.useGlobal, true);
assert.strictEqual(r2.ignoreUndefined, true);
assert.strictEqual(r2.writer, writer);
assert.strictEqual(r2.replMode, repl.REPL_MODE_STRICT);
assert.strictEqual(r2.displayWelcomeMessage, false);
assert.strictEqual(r2.executeOnTimeout, 500);

// test r2 for backwards compact
assert.strictEqual(r2.rli.input, stream);
assert.strictEqual(r2.rli.output, stream);
assert.strictEqual(r2.rli.input, r2.inputStream);
assert.strictEqual(r2.rli.output, r2.outputStream);
assert.strictEqual(r2.rli.terminal, false);

// testing out "magic" replMode
const r3 = repl.start({
  input: stream,
  output: stream,
  writer: writer,
  replMode: repl.REPL_MODE_MAGIC,
  historySize: 50
});

assert.strictEqual(r3.replMode, repl.REPL_MODE_MAGIC);
assert.strictEqual(r3.historySize, 50);

// Verify that defaults are used when no arguments are provided
const r4 = repl.start();

assert.strictEqual(r4._prompt, '> ');
assert.strictEqual(r4.input, process.stdin);
assert.strictEqual(r4.output, process.stdout);
assert.strictEqual(r4.terminal, !!r4.output.isTTY);
assert.strictEqual(r4.useColors, r4.terminal);
assert.strictEqual(r4.useGlobal, false);
assert.strictEqual(r4.ignoreUndefined, false);
assert.strictEqual(r4.replMode, repl.REPL_MODE_SLOPPY);
assert.strictEqual(r4.historySize, 30);
assert.strictEqual(r4.displayWelcomeMessage, false);
assert.strictEqual(r4.executeOnTimeout, 50);
r4.close();
