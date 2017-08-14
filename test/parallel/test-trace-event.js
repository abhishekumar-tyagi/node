'use strict';
const common = require('../common');
const assert = require('assert');
const cp = require('child_process');
const fs = require('fs');

const CODE =
  'setTimeout(() => { for (var i = 0; i < 100000; i++) { "test" + i } }, 1)';
const FILE_NAME = 'node_trace.1.log';

common.refreshTmpDir();
process.chdir(common.tmpDir);

const proc_no_categories = cp.spawn(
  process.execPath,
  [ '--trace-events-enabled', '--trace-event-categories', '""', '-e', CODE ]
);

proc_no_categories.once('exit', common.mustCall(() => {
  assert(!common.fileExists(FILE_NAME));

  const proc = cp.spawn(process.execPath,
                        [ '--trace-events-enabled', '-e', CODE ]);

  proc.once('exit', common.mustCall(() => {
    assert(common.fileExists(FILE_NAME));
    fs.readFile(FILE_NAME, common.mustCall((err, data) => {
      const traces = JSON.parse(data.toString()).traceEvents;
      assert(traces.length > 0);
      // V8 trace events should be generated.
      assert(traces.some((trace) => {
        if (trace.pid !== proc.pid)
          return false;
        if (trace.cat !== 'v8')
          return false;
        if (trace.name !== 'V8.ScriptCompiler')
          return false;
        return true;
      }));
      // Async wrap trace events should be generated.
      assert(traces.some((trace) => {
        if (trace.pid !== proc.pid)
          return false;
        if (trace.cat !== 'node')
          return false;
        if (trace.name !== 'TIMERWRAP')
          return false;
        return true;
      }));
    }));
  }));
}));
