'use strict';

require('../common');

const runBenchmark = require('../common/benchmark');

runBenchmark('process',
             [
               'millions=0.000001',
               'n=1',
               'type=raw'
             ]);
