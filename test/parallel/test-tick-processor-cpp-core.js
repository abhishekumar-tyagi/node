'use strict';
const common = require('../common');
const path = require('path');

if (common.isWindows ||
    common.isSunOS ||
    common.isAix ||
    common.isLinuxPPCBE ||
    common.isFreeBSD) {
  common.skip('C++ symbols are not mapped for this os.');
  return;
}

if (!common.enoughTestCpu) {
  common.skip('test is CPU-intensive');
  return;
}

const base = require(path.join(common.fixturesDir, 'tick-processor-base.js'));

base.runTest({
  pattern: /RunInDebugContext/,
  code: `function f() {
           require(\'vm\').runInDebugContext(\'Debug\');
           setImmediate(function() { f(); });
         };
         f();`
});
