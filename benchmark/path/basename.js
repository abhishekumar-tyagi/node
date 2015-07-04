var common = require('../common.js');
var path = require('path');
var v8 = require('v8');

var bench = common.createBenchmark(main, {
  type: ['win32', 'posix'],
  n: [1e6],
});

function main(conf) {
  var n = +conf.n;
  var p = path[conf.type];

  // Force optimization before starting the benchmark
  p.basename('/foo/bar/baz/asdf/quux.html');
  v8.setFlagsFromString('--allow_natives_syntax');
  eval('%OptimizeFunctionOnNextCall(p.basename)');
  p.basename('/foo/bar/baz/asdf/quux.html');

  bench.start();
  for (var i = 0; i < n; i++) {
    p.basename('/foo/bar/baz/asdf/quux.html');
    p.basename('/foo/bar/baz/asdf/quux.html', '.html');
  }
  bench.end(n);
}
