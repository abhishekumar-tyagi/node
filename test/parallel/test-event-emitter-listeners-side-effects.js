'use strict';
// Flags: --expose-internals
var common = require('../common');
var assert = require('assert');
var events = require('events');
const EEEvents = require('internal/symbols').EEEvents;

var EventEmitter = require('events').EventEmitter;

var e = new EventEmitter();
var fl;  // foo listeners

fl = e.listeners('foo');
assert(Array.isArray(fl));
assert(fl.length === 0);
assert.strictEqual(e[EEEvents].size, 0);

e.on('foo', assert.fail);
fl = e.listeners('foo');
assert(e[EEEvents].get('foo') === assert.fail);
assert(Array.isArray(fl));
assert(fl.length === 1);
assert(fl[0] === assert.fail);

e.listeners('bar');
assert(!e[EEEvents].has('bar'));

e.on('foo', assert.ok);
fl = e.listeners('foo');

const foo = e[EEEvents].get('foo');
assert(Array.isArray(foo));
assert(foo.length === 2);
assert(foo[0] === assert.fail);
assert(foo[1] === assert.ok);

assert(Array.isArray(fl));
assert(fl.length === 2);
assert(fl[0] === assert.fail);
assert(fl[1] === assert.ok);

console.log('ok');
