'use strict';

const common = require('../common.js');
const assert = require('assert');

const bench = common.createBenchmark(main, {
  n: [25, 2e2, 2e3],
  size: [1e2, 1e3, 1e4],
  strict: [1],
  method: ['deepEqual', 'notDeepEqual'],
}, {
  combinationFilter: (p) => {
    return p.size === 1e4 && p.n === 25 ||
           p.size === 1e3 && p.n === 2e2 ||
           p.size === 1e2 && p.n === 2e3;
  },
});

function createObj(size, add = '') {
  return Array.from({ length: size }, (n) => ({
    foo: 'yarp',
    nope: {
      bar: `123${add}`,
      a: [1, 2, 3],
      baz: n,
      c: {},
      b: [],
    },
  }));
}

function main({ size, n, method, strict }) {
  if (strict) {
    method = method.replace('eep', 'eepStrict');
  }
  const fn = assert[method];

  const actual = createObj(size);
  const expected = method.includes('not') ? createObj(size, '4') : createObj(size);

  bench.start();
  for (let i = 0; i < n; ++i) {
    fn(actual, expected);
  }
  bench.end(n);
}
