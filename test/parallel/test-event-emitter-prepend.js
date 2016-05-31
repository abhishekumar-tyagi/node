'use strict';

const common = require('../common');
const EventEmitter = require('events');
const assert = require('assert');

const myEE = new EventEmitter();
var m = 0;
// This one comes last.
myEE.on('foo', common.mustCall(() => assert.equal(m, 2)));

// This one comes second.
myEE.prependListener('foo', common.mustCall(() => assert.equal(m++, 1)));

// This one comes first.
myEE.prependOnceListener('foo', common.mustCall(() => assert.equal(m++, 0)));

myEE.emit('foo');


// Test fallback if prependListener is undefined.
const stream = require('stream');

delete EventEmitter.prototype.prependListener;

class Writable extends stream.Stream {
  constructor() {
    super();
    this.writable = true;
  }
}

class Readable extends stream.Stream {
  constructor() {
    super();
    this.readable = true;
  }
}

const w = new Writable();
const r = new Readable();
r.pipe(w);
