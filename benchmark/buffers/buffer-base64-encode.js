'use strict';
var common = require('../common.js');

const bench = common.createBenchmark(main, {
  N: [64 * 1024 * 1024],
  n: [32]
});

function main(conf) {
  var n = conf.n;
  var N = conf.N;
  var b = Buffer.allocUnsafe(N);
  var s = '';
  var i;
  for (i = 0; i < 256; ++i) s += String.fromCharCode(i);
  for (i = 0; i < N; i += 256) b.write(s, i, 256, 'ascii');
  bench.start();
  for (i = 0; i < n; ++i) b.toString('base64');
  bench.end(n);
}
