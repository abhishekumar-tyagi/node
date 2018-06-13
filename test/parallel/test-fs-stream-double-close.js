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
const { createReadStream, createWriteStream } = require('fs');

const tmpdir = require('../common/tmpdir');
tmpdir.refresh();

test1(createReadStream(__filename));
test2(createReadStream(__filename));
test3(createReadStream(__filename));

test1(createWriteStream(`${tmpdir.path}/dummy1`));
test2(createWriteStream(`${tmpdir.path}/dummy2`));
test3(createWriteStream(`${tmpdir.path}/dummy3`));

function test1(stream) {
  stream.destroy();
  stream.destroy();
}

function test2(stream) {
  stream.destroy();
  stream.on('open', common.mustCall(function(fd) {
    stream.destroy();
  }));
}

function test3(stream) {
  stream.on('open', common.mustCall(function(fd) {
    stream.destroy();
    stream.destroy();
  }));
}
