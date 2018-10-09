'use strict';

const common = require('../common.js');

const bench = common.createBenchmark(main, {
  n: [100000],
  method: ['trace', 'isTraceCategoryEnabled']
}, {
  flags: ['--expose-internals', '--trace-event-categories', 'foo']
});

const {
  TRACE_EVENT_PHASE_NESTABLE_ASYNC_BEGIN: kBeforeEvent
} = process.binding('constants').trace;

function doTrace(n, trace) {
  bench.start();
  for (var i = 0; i < n; i++) {
    trace(kBeforeEvent, 'foo', 'test', 0, 'test');
  }
  bench.end(n);
}

function doIsTraceCategoryEnabled(n, isTraceCategoryEnabled) {
  bench.start();
  for (var i = 0; i < n; i++) {
    isTraceCategoryEnabled('foo');
    isTraceCategoryEnabled('bar');
  }
  bench.end(n);
}

function main({ n, method }) {
  const { internalBinding } = require('internal/test/binding');

  const {
    trace,
    isTraceCategoryEnabled
  } = internalBinding('trace_events');

  switch (method) {
    case '':
    case 'trace':
      doTrace(n, trace);
      break;
    case 'isTraceCategoryEnabled':
      doIsTraceCategoryEnabled(n, isTraceCategoryEnabled);
      break;
    default:
      throw new Error(`Unexpected method "${method}"`);
  }
}
