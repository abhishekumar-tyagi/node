'use strict';
const common = require('../common');
const assert = require('assert');
const cluster = require('cluster');

if (cluster.isMaster) {
  /*
   * By default, the `-r` and `./test/common` will be passed, to preload common
   * module. So, we are making sure that no other arguments are passed.
   */
  assert.strictEqual(process.execArgv.length, 2, 'run test with no args');
  assert.deepStrictEqual(process.execArgv, ['-r', './test/common']);

  function checkExitCode(code, signal) {
    assert.strictEqual(code, 0);
    assert.strictEqual(signal, null);
  }

  console.log('forked worker should not have --debug-port');
  cluster.fork().on('exit', checkExitCode);

  cluster.setupMaster({
    execArgv: ['--debug-port=' + process.debugPort]
  });

  console.log('forked worker should have --debug-port, with offset = 1');
  cluster.fork({
    portSet: process.debugPort + 1
  }).on('exit', checkExitCode);

  console.log('forked worker should have --debug-port, with offset = 2');
  cluster.fork({
    portSet: process.debugPort + 2
  }).on('exit', checkExitCode);
} else {
  const hasDebugArg = process.execArgv.some(function(arg) {
    return /debug/.test(arg);
  });

  assert.strictEqual(hasDebugArg, process.env.portSet !== undefined);
  assert.strictEqual(process.debugPort, +process.env.portSet || 5858);
  process.exit();
}
