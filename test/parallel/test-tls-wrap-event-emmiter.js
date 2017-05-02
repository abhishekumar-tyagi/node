'use strict';

/*
 * Issue: https://github.com/nodejs/node/issues/3655
 * Test checks if we get exception instead of runtime error
 */

require('../common');
const assert = require('assert');

const TlsSocket = require('tls').TLSSocket;
const EventEmitter = require('events').EventEmitter;
assert.throws(
  () => { new TlsSocket(new EventEmitter()); },
  /^TypeError: this\.stream\.pause is not a function/
);
