'use strict';

const common = require('../common.js');
const { deepEqual, deepStrictEqual, notDeepEqual, notDeepStrictEqual } =
  require('assert');

const bench = common.createBenchmark(main, {
  n: [5e2],
  len: [5e2],
  strict: [0, 1],
  method: [
    'deepEqual_primitiveOnly',
    'deepEqual_objectOnly',
    'deepEqual_mixed',
    'notDeepEqual_primitiveOnly',
    'notDeepEqual_objectOnly',
    'notDeepEqual_mixed',
  ],
});

function benchmark(method, n, values, values2) {
  const actual = new Map(values);
  // Prevent reference equal elements
  const deepCopy = JSON.parse(JSON.stringify(values2 ? values2 : values));
  const expected = new Map(deepCopy);
  bench.start();
  for (let i = 0; i < n; ++i) {
    method(actual, expected);
  }
  bench.end(n);
}

function main({ n, len, method, strict }) {
  const array = Array(len).fill(1);
  let values, values2;

  switch (method) {
    case '':
      // Empty string falls through to next line as default, mostly for tests.
    case 'deepEqual_primitiveOnly':
      values = array.map((_, i) => [`str_${i}`, 123]);
      benchmark(strict ? deepStrictEqual : deepEqual, n, values);
      break;
    case 'deepEqual_objectOnly':
      values = array.map((_, i) => [[`str_${i}`, 1], 123]);
      benchmark(strict ? deepStrictEqual : deepEqual, n, values);
      break;
    case 'deepEqual_mixed':
      values = array.map((_, i) => [i % 2 ? [`str_${i}`, 1] : `str_${i}`, 123]);
      benchmark(strict ? deepStrictEqual : deepEqual, n, values);
      break;
    case 'notDeepEqual_primitiveOnly':
      values = array.map((_, i) => [`str_${i}`, 123]);
      values2 = values.slice(0);
      values2[Math.floor(len / 2)] = ['w00t', 123];
      benchmark(strict ? notDeepStrictEqual : notDeepEqual, n, values, values2);
      break;
    case 'notDeepEqual_objectOnly':
      values = array.map((_, i) => [[`str_${i}`, 1], 123]);
      values2 = values.slice(0);
      values2[Math.floor(len / 2)] = [['w00t'], 123];
      benchmark(strict ? notDeepStrictEqual : notDeepEqual, n, values, values2);
      break;
    case 'notDeepEqual_mixed':
      values = array.map((_, i) => [i % 2 ? [`str_${i}`, 1] : `str_${i}`, 123]);
      values2 = values.slice(0);
      values2[0] = ['w00t', 123];
      benchmark(strict ? notDeepStrictEqual : notDeepEqual, n, values, values2);
      break;
    default:
      throw new Error(`Unsupported method ${method}`);
  }
}
