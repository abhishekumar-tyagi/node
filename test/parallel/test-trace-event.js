'use strict';

const assert = require('assert');
const cp = require('child_process');
const common = require('../common.js');
const fs = require('fs');

const CODE = 'for (var i = 0; i < 100000; i++) { \"test\" + i }';
const FILE_NAME = 'node_trace.1.log';

common.refreshTmpDir();
process.chdir(common.tmpDir);

const proc = cp.spawn(process.execPath,
  [ '--enable-tracing', '-e', CODE ]);

proc.once('exit', common.mustCall(() => {
  assert(common.fileExists(FILE_NAME));
  fs.readFile(FILE_NAME, (err, data) => {
    const traces = JSON.parse(data).traceEvents;
    assert(traces.length > 0);
    // Values that should be present in all outputs to approximate well-formedness.
    assert(traces.some((trace) => { return trace.pid === proc.pid; }));
    assert(traces.some((trace) => { return trace.cat === 'v8'; }));
    assert(traces.some((trace) => { return trace.name === 'V8.ScriptCompiler'; }));
  });
}));
