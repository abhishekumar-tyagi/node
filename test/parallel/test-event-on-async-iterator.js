'use strict';

const common = require('../common');
const assert = require('assert');
const { on, EventEmitter } = require('events');

async function basic() {
  const ee = new EventEmitter();
  process.nextTick(() => {
    ee.emit('foo', 'bar');
    // 'bar' is a spurious event, we are testing
    // that it does not show up in the iterable
    ee.emit('bar', 24);
    ee.emit('foo', 42);
  });

  const iterable = on(ee, 'foo');

  const expected = [['bar'], [42]];

  for await (const event of iterable) {
    const current = expected.shift();

    assert.deepStrictEqual(current, event);

    if (expected.length === 0) {
      break;
    }
  }
  assert.strictEqual(ee.listenerCount('foo'), 0);
  assert.strictEqual(ee.listenerCount('error'), 0);
}

async function error() {
  const ee = new EventEmitter();
  const _err = new Error('kaboom');
  process.nextTick(() => {
    ee.emit('error', _err);
  });

  const iterable = on(ee, 'foo');
  let looped = false;
  let thrown = false;

  try {
    // eslint-disable-next-line no-unused-vars
    for await (const event of iterable) {
      looped = true;
    }
  } catch (err) {
    thrown = true;
    assert.strictEqual(err, _err);
  }
  assert.strictEqual(thrown, true);
  assert.strictEqual(looped, false);
}

async function errorDelayed() {
  const ee = new EventEmitter();
  const _err = new Error('kaboom');
  process.nextTick(() => {
    ee.emit('foo', 42);
    ee.emit('error', _err);
  });

  const iterable = on(ee, 'foo');
  const expected = [[42]];
  let thrown = false;

  try {
    for await (const event of iterable) {
      const current = expected.shift();
      assert.deepStrictEqual(current, event);
    }
  } catch (err) {
    thrown = true;
    assert.strictEqual(err, _err);
  }
  assert.strictEqual(thrown, true);
  assert.strictEqual(ee.listenerCount('foo'), 0);
  assert.strictEqual(ee.listenerCount('error'), 0);
}

async function throwInLoop() {
  const ee = new EventEmitter();
  const _err = new Error('kaboom');

  process.nextTick(() => {
    ee.emit('foo', 42);
  });

  try {
    for await (const event of on(ee, 'foo')) {
      assert.deepStrictEqual(event, [42]);
      throw _err;
    }
  } catch (err) {
    assert.strictEqual(err, _err);
  }

  assert.strictEqual(ee.listenerCount('foo'), 0);
  assert.strictEqual(ee.listenerCount('error'), 0);
}

async function next() {
  const ee = new EventEmitter();
  const iterable = on(ee, 'foo');
  process.nextTick(function() {
    ee.emit('foo', 'bar');
    ee.emit('foo', 42);
    iterable.return();
  });
  const results = await Promise.all([
    iterable.next(),
    iterable.next(),
    iterable.next()
  ]);
  assert.deepStrictEqual(results, [{
    value: ['bar'],
    done: false
  }, {
    value: [42],
    done: false
  }, {
    value: undefined,
    done: true
  }]);
}

async function nextError() {
  const ee = new EventEmitter();
  const iterable = on(ee, 'foo');
  const _err = new Error('kaboom');
  process.nextTick(function() {
    ee.emit('error', _err);
  });
  const results = await Promise.allSettled([
    iterable.next(),
    iterable.next(),
    iterable.next()
  ]);
  assert.deepStrictEqual(results, [{
    status: 'rejected',
    reason: _err
  }, {
    status: 'rejected',
    reason: _err
  }, {
    status: 'rejected',
    reason: _err
  }]);
}

async function iterableThrow() {
  const ee = new EventEmitter();
  process.nextTick(() => {
    ee.emit('foo', 'bar');
    ee.emit('foo', 42); // lost in the queue
  });

  const iterable = on(ee, 'foo');
  const _err = new Error('kaboom');
  let thrown = false;

  try {
    for await (const event of iterable) {
      assert.deepStrictEqual(event, ['bar']);
      assert.throws(() => {
        // No argument
        iterable.throw();
      }, {
        message: 'The "EventEmitter.AsyncIterator" property must be' +
                 ' of type Error. Received type undefined',
        name: 'TypeError'
      });
      iterable.throw(_err);
    }
  } catch (err) {
    thrown = true;
    assert.strictEqual(err, _err);
  }
  assert.strictEqual(thrown, true);
  assert.strictEqual(ee.listenerCount('foo'), 0);
  assert.strictEqual(ee.listenerCount('error'), 0);
}

async function run() {
  const funcs = [
    basic,
    error,
    errorDelayed,
    throwInLoop,
    next,
    nextError,
    iterableThrow
  ]

  for (const fn of funcs) {
    await fn()
  }
}

run().then(common.mustCall());
