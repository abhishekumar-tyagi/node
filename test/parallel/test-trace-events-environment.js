'use strict';
const common = require('../common');
const assert = require('assert');
const cp = require('child_process');
const path = require('path');
const fs = require('fs');
const tmpdir = require('../common/tmpdir');

// This tests the emission of node.environment trace events

if (!common.isMainThread)
  common.skip('process.chdir is not available in Workers');

const names = new Set([
  'Environment',
  'RunAndClearNativeImmediates',
  'CheckImmediate',
  'RunTimers',
  'BeforeExit',
  'RunCleanup',
  'AtExit'
]);

if (process.argv[2] === 'child') {
  // This is just so that the child has something to do.
  1 + 1;
  // These ensure that the RunTimers, CheckImmediate, and
  // RunAndClearNativeImmediates appear in the list.
  setImmediate(() => { 1 + 1; });
  setTimeout(() => { 1 + 1; }, 1);
} else {
  tmpdir.refresh();
  process.chdir(tmpdir.path);

  const proc = cp.fork(__filename,
                       [ 'child' ], {
                         execArgv: [
                           '--trace-event-categories',
                           'node.environment'
                         ]
                       });

  proc.once('exit', common.mustCall(async () => {
    const file = path.join(tmpdir.path, 'node_trace.1.log');
    const checkSet = new Set();

    assert(fs.existsSync(file));
    const data = await fs.promises.readFile(file);
    JSON.parse(data.toString()).traceEvents
      .filter((trace) => trace.cat !== '__metadata')
      .forEach((trace) => {
        assert.strictEqual(trace.pid, proc.pid);
        assert(names.has(trace.name));
        checkSet.add(trace.name);
      });

    assert.deepStrictEqual(names, checkSet);
  }));
}
