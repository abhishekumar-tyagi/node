'use strict';
require('../common');
const assert = require('assert');

console.error('load test-module-loading-error.js');

var error_desc = {
  win32: ['%1 is not a valid Win32 application'],
  linux: ['file too short', 'Exec format error'],
  sunos: ['unknown file type', 'not an ELF file'],
  darwin: ['file too short']
};
var dlerror_msg = error_desc[process.platform];

if (!dlerror_msg) {
  console.log('1..0 # Skipped: platform not supported.');
  return;
}

try {
  require('../fixtures/module-loading-error.node');
} catch (e) {
  assert.strictEqual(dlerror_msg.some((errMsgCase) => {
    return e.toString().indexOf(errMsgCase) !== -1;
  }), true);
}

try {
  require();
} catch (e) {
  assert(/^missing path/.test(e.message));
}

try {
  require({});
} catch (e) {
  assert(/^path must be a string/.test(e.message));
}
