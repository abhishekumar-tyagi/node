'use strict';
const common = require('../common.js');

const bench = common.createBenchmark(main, {
  type: ['Double', 'Float'],
  endian: ['BE', 'LE'],
  value: ['zero', 'big', 'small', 'inf', 'nan'],
  millions: [1],
});

function main({ millions, type, endian, value }) {
  type = type || 'Double';
  const buff = Buffer.alloc(8);
  const fn = `read${type}${endian}`;
  const values = {
    Double: {
      zero: 0,
      big: 2 ** 1023,
      small: 2 ** -1074,
      inf: Infinity,
      nan: NaN,
    },
    Float: {
      zero: 0,
      big: 2 ** 127,
      small: 2 ** -149,
      inf: Infinity,
      nan: NaN,
    },
  };

  buff[`write${type}${endian}`](values[type][value], 0);

  bench.start();
  for (var i = 0; i !== millions * 1e6; i++) {
    buff[fn](0);
  }
  bench.end(millions);
}
