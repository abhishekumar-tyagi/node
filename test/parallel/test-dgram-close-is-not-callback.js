'use strict';
const common = require('../common');
const dgram = require('dgram');

const buf = new Buffer(1024);
buf.fill(42);

const socket = dgram.createSocket('udp4');

socket.send(buf, 0, buf.length, common.PORT, 'localhost');

// if close callback is not function, ignore the argument.
socket.close('bad argument');

socket.on('close', common.mustCall(function() {}));
