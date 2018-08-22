// Flags: --expose-internals
'use strict';
const common = require('../common');
const assert = require('assert');
const { _createSocketHandle } = require('internal/dgram');
const UDP = process.binding('udp_wrap').UDP;

{
  // Create a handle that is not bound.
  const handle = _createSocketHandle(null, null, 'udp4');

  assert(handle instanceof UDP);
  assert.strictEqual(typeof handle.fd, 'number');
  assert(handle.fd < 0);
}

{
  // Create a bound handle.
  const handle = _createSocketHandle(common.localhostIPv4, 0, 'udp4');

  assert(handle instanceof UDP);
  assert.strictEqual(typeof handle.fd, 'number');

  if (process.platform !== 'win32')
    assert(handle.fd > 0);
}

{
  // Return an error if binding fails.
  const err = _createSocketHandle('localhost', 0, 'udp4');

  assert.strictEqual(typeof err, 'number');
  assert(err < 0);
}
