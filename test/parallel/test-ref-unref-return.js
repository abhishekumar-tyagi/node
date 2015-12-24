'use strict';
require('../common');
var assert = require('assert');
var net = require('net');
var dgram = require('dgram');

assert.ok((new net.Server()).ref() instanceof net.Server);
assert.ok((new net.Server()).unref() instanceof net.Server);
assert.ok((new net.Socket()).ref() instanceof net.Socket);
assert.ok((new net.Socket()).unref() instanceof net.Socket);
assert.ok((new dgram.Socket('udp4')).ref() instanceof dgram.Socket);
assert.ok((new dgram.Socket('udp6')).unref() instanceof dgram.Socket);
