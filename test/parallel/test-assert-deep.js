'use strict';
require('../common');
const assert = require('assert');
const util = require('util');

// Template tag function turning an error message into a RegExp
// for assert.throws()
function re(literals, ...values) {
  let result = literals[0];
  for (const [i, value] of values.entries()) {
    const str = util.inspect(value);
    // Need to escape special characters.
    result += str.replace(/[\\^$.*+?()[\]{}|=!<>:-]/g, '\\$&');
    result += literals[i + 1];
  }
  return new RegExp(`^AssertionError: ${result}$`);
}

// The following deepEqual tests might seem very weird.
// They just describe what it is now.
// That is why we discourage using deepEqual in our own tests.

// Turn off no-restricted-properties because we are testing deepEqual!
/* eslint-disable no-restricted-properties */

const arr = new Uint8Array([120, 121, 122, 10]);
const buf = Buffer.from(arr);
// They have different [[Prototype]]
assert.throws(() => assert.deepStrictEqual(arr, buf));
assert.doesNotThrow(() => assert.deepEqual(arr, buf));

const buf2 = Buffer.from(arr);
buf2.prop = 1;

assert.throws(() => assert.deepStrictEqual(buf2, buf));
assert.doesNotThrow(() => assert.deepEqual(buf2, buf));

const arr2 = new Uint8Array([120, 121, 122, 10]);
arr2.prop = 5;
assert.throws(() => assert.deepStrictEqual(arr, arr2));
assert.doesNotThrow(() => assert.deepEqual(arr, arr2));

const date = new Date('2016');

class MyDate extends Date {
  constructor(...args) {
    super(...args);
    this[0] = '1';
  }
}

const date2 = new MyDate('2016');

// deepEqual returns true as long as the time are the same,
// but deepStrictEqual checks own properties
assert.doesNotThrow(() => assert.deepEqual(date, date2));
assert.doesNotThrow(() => assert.deepEqual(date2, date));
assert.throws(() => assert.deepStrictEqual(date, date2),
              re`${date} deepStrictEqual ${date2}`);
assert.throws(() => assert.deepStrictEqual(date2, date),
              re`${date2} deepStrictEqual ${date}`);

class MyRegExp extends RegExp {
  constructor(...args) {
    super(...args);
    this[0] = '1';
  }
}

const re1 = new RegExp('test');
const re2 = new MyRegExp('test');

// deepEqual returns true as long as the regexp-specific properties
// are the same, but deepStrictEqual checks all properties
assert.doesNotThrow(() => assert.deepEqual(re1, re2));
assert.throws(() => assert.deepStrictEqual(re1, re2),
              re`${re1} deepStrictEqual ${re2}`);

// For these weird cases, deepEqual should pass (at least for now),
// but deepStrictEqual should throw.
const similar = new Set([
  {0: '1'},  // Object
  {0: 1},  // Object
  new String('1'),  // Object
  ['1'],  // Array
  [1],  // Array
  date2,  // Date with this[0] = '1'
  re2,  // RegExp with this[0] = '1'
  new Int8Array([1]), // Int8Array
  new Uint8Array([1]), // Uint8Array
  new Int16Array([1]), // Int16Array
  new Uint16Array([1]), // Uint16Array
  new Int32Array([1]), // Int32Array
  new Uint32Array([1]), // Uint32Array
  Buffer.from([1]),
  // Arguments {'0': '1'} is not here
  // See https://github.com/nodejs/node-v0.x-archive/pull/7178
]);

for (const a of similar) {
  for (const b of similar) {
    if (a !== b) {
      assert.deepEqual(a, b);
      assert.throws(() => assert.deepStrictEqual(a, b),
                    re`${a} deepStrictEqual ${b}`);
    }
  }
}

function assertDeepAndStrictEqual(a, b) {
  assert.doesNotThrow(() => assert.deepEqual(a, b));
  assert.doesNotThrow(() => assert.deepStrictEqual(a, b));

  assert.doesNotThrow(() => assert.deepEqual(b, a));
  assert.doesNotThrow(() => assert.deepStrictEqual(b, a));
}

function assertNotDeepOrStrict(a, b) {
  assert.throws(() => assert.deepEqual(a, b));
  assert.throws(() => assert.deepStrictEqual(a, b));

  assert.throws(() => assert.deepEqual(b, a));
  assert.throws(() => assert.deepStrictEqual(b, a));
}

function assertOnlyDeepEqual(a, b) {
  assert.doesNotThrow(() => assert.deepEqual(a, b));
  assert.throws(() => assert.deepStrictEqual(a, b));

  assert.doesNotThrow(() => assert.deepEqual(b, a));
  assert.throws(() => assert.deepStrictEqual(b, a));
}

// es6 Maps and Sets
assertDeepAndStrictEqual(new Set(), new Set());
assertDeepAndStrictEqual(new Map(), new Map());

assertDeepAndStrictEqual(new Set([1, 2, 3]), new Set([1, 2, 3]));
assertNotDeepOrStrict(new Set([1, 2, 3]), new Set([1, 2, 3, 4]));
assertNotDeepOrStrict(new Set([1, 2, 3, 4]), new Set([1, 2, 3]));
assertDeepAndStrictEqual(new Set(['1', '2', '3']), new Set(['1', '2', '3']));
assertDeepAndStrictEqual(new Set([[1, 2], [3, 4]]), new Set([[3, 4], [1, 2]]));

assertDeepAndStrictEqual(new Map([[1, 1], [2, 2]]), new Map([[1, 1], [2, 2]]));
assertDeepAndStrictEqual(new Map([[1, 1], [2, 2]]), new Map([[2, 2], [1, 1]]));
assertNotDeepOrStrict(new Map([[1, 1], [2, 2]]), new Map([[1, 2], [2, 1]]));

assertNotDeepOrStrict(new Set([1]), [1]);
assertNotDeepOrStrict(new Set(), []);
assertNotDeepOrStrict(new Set(), {});

assertNotDeepOrStrict(new Map([['a', 1]]), {a: 1});
assertNotDeepOrStrict(new Map(), []);
assertNotDeepOrStrict(new Map(), {});

assertOnlyDeepEqual(new Set(['1']), new Set([1]));

assertOnlyDeepEqual(new Map([['1', 'a']]), new Map([[1, 'a']]));
assertOnlyDeepEqual(new Map([['a', '1']]), new Map([['a', 1]]));

// This is an awful case, where a map contains multiple equivalent keys:
assertOnlyDeepEqual(
  new Map([[1, 'a'], ['1', 'b']]),
  new Map([['1', 'a'], [1, 'b']])
);
assertDeepAndStrictEqual(
  new Map([[{}, 'a'], [{}, 'b']]),
  new Map([[{}, 'b'], [{}, 'a']])
);

{
  const values = [
    123,
    Infinity,
    0,
    null,
    undefined,
    false,
    true,
    {},
    [],
    () => {},
  ];
  assertDeepAndStrictEqual(new Set(values), new Set(values));
  assertDeepAndStrictEqual(new Set(values), new Set(values.reverse()));

  const mapValues = values.map((v) => [v, {a: 5}]);
  assertDeepAndStrictEqual(new Map(mapValues), new Map(mapValues));
  assertDeepAndStrictEqual(new Map(mapValues), new Map(mapValues.reverse()));
}

{
  const s1 = new Set();
  const s2 = new Set();
  s1.add(1);
  s1.add(2);
  s2.add(2);
  s2.add(1);
  assertDeepAndStrictEqual(s1, s2);
}

{
  const m1 = new Map();
  const m2 = new Map();
  const obj = {a: 5, b: 6};
  m1.set(1, obj);
  m1.set(2, 'hi');
  m1.set(3, [1, 2, 3]);

  m2.set(2, 'hi'); // different order
  m2.set(1, obj);
  m2.set(3, [1, 2, 3]); // deep equal, but not reference equal.

  assertDeepAndStrictEqual(m1, m2);
}

{
  const m1 = new Map();
  const m2 = new Map();

  // m1 contains itself.
  m1.set(1, m1);
  m2.set(1, new Map());

  assertNotDeepOrStrict(m1, m2);
}

assert.deepEqual(new Map([[1, 1]]), new Map([[1, '1']]));
assert.throws(() =>
  assert.deepStrictEqual(new Map([[1, 1]]), new Map([[1, '1']]))
);

{
  // Two equivalent sets / maps with different key/values applied shouldn't be
  // the same. This is a terrible idea to do in practice, but deepEqual should
  // still check for it.
  const s1 = new Set();
  const s2 = new Set();
  s1.x = 5;
  assertNotDeepOrStrict(s1, s2);

  const m1 = new Map();
  const m2 = new Map();
  m1.x = 5;
  assertNotDeepOrStrict(m1, m2);
}

{
  // Circular references.
  const s1 = new Set();
  s1.add(s1);
  const s2 = new Set();
  s2.add(s2);
  assertDeepAndStrictEqual(s1, s2);

  const m1 = new Map();
  m1.set(2, m1);
  const m2 = new Map();
  m2.set(2, m2);
  assertDeepAndStrictEqual(m1, m2);

  const m3 = new Map();
  m3.set(m3, 2);
  const m4 = new Map();
  m4.set(m4, 2);
  assertDeepAndStrictEqual(m3, m4);
}

/* eslint-enable */
