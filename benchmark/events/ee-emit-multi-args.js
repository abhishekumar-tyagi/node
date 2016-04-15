'use strict';

var common = require('../common');
var EventEmitter = require('events');
var v8 = require('v8');

var bench = common.createBenchmark(main, {
  n: [25e6]
});

function main(conf) {
  var n = conf.n | 0;

  var ee = new EventEmitter();

  for (var k = 0; k < 10; k += 1)
    ee.on('dummy', function() {});

  ee.emit('dummy', 5, true);
  v8.setFlagsFromString('--allow_natives_syntax');
  eval('%OptimizeFunctionOnNextCall(ee.emit)');
  ee.emit('dummy', 5, true);

  bench.start();
  for (var i = 0; i < n; i += 1) {
    ee.emit('dummy', 5, true);
  }
  bench.end(n);
}
