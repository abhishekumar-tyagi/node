// Flags: --expose-internals
'use strict';
const common = require('../common');
const { internalBinding } = require('internal/test/binding');
const { signals } = internalBinding('constants').os;
const Signal = internalBinding('signal_wrap').Signal;

if (!common.isMainThread)
  common.skip('Signal handling in Workers is not supported');

const signal = new Signal();
signal.start(signals.SIGUSR2);
signal.onsignal = common.mustCall(() => {
  process.nextTick(() => {
    signal.start(signals.SIGINT);
    signal.onsignal = common.mustCall(() => {
      signal.close();
    });
    process.kill(0, signals.SIGINT);
  });
});
process.kill(0, signals.SIGUSR2);
