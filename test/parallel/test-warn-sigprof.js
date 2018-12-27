'use strict';
const common = require('../common');

// The inspector attempts to start when Node starts. Once started, the inspector
// warns on the use of a SIGPROF listener.

common.skipIfInspectorDisabled();

if (common.isWindows)
  common.skip('test does not apply to Windows');

common.skipIfWorker(); // Worker inspector never has a server running

common.requireFlags(['--inspect=0']);

common.expectWarning('Warning',
                     'process.on(SIGPROF) is reserved while debugging',
                     common.noWarnCode);

process.on('SIGPROF', () => {});
