'use strict';
const common = require('../common');
if (!global.gc)
  common.relaunchWithFlags(['--expose-gc']);
const onGC = require('../common/ongc');

{
  onGC({}, { ongc: common.mustCall() });
  global.gc();
}

{
  onGC(process, { ongc: common.mustNotCall() });
  global.gc();
}
