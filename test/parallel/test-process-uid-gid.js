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

if (common.isWindows) {
  // uid/gid functions are POSIX only.
  assert.strictEqual(process.getuid, undefined);
  assert.strictEqual(process.getgid, undefined);
  assert.strictEqual(process.setuid, undefined);
  assert.strictEqual(process.setgid, undefined);
  assert.strictEqual(process.getresuid, undefined);
  assert.strictEqual(process.setresuid, undefined);
  return;
}

if (!common.isMainThread)
  return;

assert.throws(() => {
  process.setuid({});
}, {
  code: 'ERR_INVALID_ARG_TYPE',
  message: 'The "id" argument must be one of type ' +
    'number or string. Received an instance of Object'
});

assert.throws(() => {
  process.setuid('fhqwhgadshgnsdhjsdbkhsdabkfabkveyb');
}, {
  code: 'ERR_UNKNOWN_CREDENTIAL',
  message: 'User identifier does not exist: fhqwhgadshgnsdhjsdbkhsdabkfabkveyb'
});

if (!common.isOSX) {
  assert.throws(() => {
    process.setresuid({}, 0, 0);
  }, {
    code: 'ERR_INVALID_ARG_TYPE',
    message: 'The "ruid" argument must be one of type ' +
      'number or string. Received an instance of Object'
  });

  assert.throws(() => {
    process.setresuid(0, {}, 0);
  }, {
    code: 'ERR_INVALID_ARG_TYPE',
    message: 'The "euid" argument must be one of type ' +
      'number or string. Received an instance of Object'
  });

  assert.throws(() => {
    process.setresuid(0, 0, {});
  }, {
    code: 'ERR_INVALID_ARG_TYPE',
    message: 'The "suid" argument must be one of type ' +
      'number or string. Received an instance of Object'
  });
}

// Passing -0 shouldn't crash the process
// Refs: https://github.com/nodejs/node/issues/32750
try { process.setuid(-0); } catch {}
try { process.seteuid(-0); } catch {}
try { process.setgid(-0); } catch {}
try { process.setegid(-0); } catch {}

// If we're not running as super user...
if (process.getuid() !== 0) {
  // Should not throw.
  process.getgid();
  process.getuid();
  if (!common.isOSX) process.getresuid();

  assert.throws(
    () => { process.setgid('nobody'); },
    /(?:EPERM, .+|Group identifier does not exist: nobody)$/
  );

  assert.throws(
    () => { process.setuid('nobody'); },
    /(?:EPERM, .+|User identifier does not exist: nobody)$/
  );

  if (!common.isOSX) {
    assert.throws(
      () => { process.setresuid('nobody', 1, 2); },
      /(?:EPERM, .+|Group identifier does not exist: \[ 'nobody' \])$/
    );
  }
  return;
}

// If we are running as super user...
const oldgid = process.getgid();
try {
  process.setgid('nobody');
} catch (err) {
  if (err.code !== 'ERR_UNKNOWN_CREDENTIAL') {
    throw err;
  }
  process.setgid('nogroup');
}

const newgid = process.getgid();
assert.notStrictEqual(newgid, oldgid);

const olduid = process.getuid();
process.setuid('nobody');
const newuid = process.getuid();
assert.notStrictEqual(newuid, olduid);
