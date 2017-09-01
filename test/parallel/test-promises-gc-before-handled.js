// Flags: --expose-gc

'use strict';
const common = require('../common');

const p = new Promise((res, rej) => {
  consol.log('oops'); // eslint-disable-line no-undef
});

// Manually call GC due to possible memory constraints with attempting to
// trigger it "naturally".
setTimeout(common.mustCall(() => {
  global.gc();
  global.gc();
  global.gc();
  setTimeout(common.mustCall(() => {
    global.gc();
    global.gc();
    global.gc();
    setTimeout(common.mustCall(() => {
      p.catch(() => {});
    }, 1), 250);
  }), 20);
}), 20);
