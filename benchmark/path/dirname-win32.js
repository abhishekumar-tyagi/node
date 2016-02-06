var common = require('../common.js');
var path = require('path');
var v8 = require('v8');

var bench = common.createBenchmark(main, {
  path: [
    '',
    '\\',
    '\\foo',
    'C:\\foo\\bar',
    'foo',
    'foo\\bar',
    'D:\\foo\\bar\\baz\\asdf\\quux'
  ],
  n: [1e6]
});

function main(conf) {
  var n = +conf.n;
  var p = path.win32;
  var input = '' + conf.path;

  // Force optimization before starting the benchmark
  p.dirname(input);
  v8.setFlagsFromString('--allow_natives_syntax');
  eval('%OptimizeFunctionOnNextCall(p.dirname)');
  p.dirname(input);

  bench.start();
  for (var i = 0; i < n; i++) {
    p.dirname(input);
  }
  bench.end(n);
}
