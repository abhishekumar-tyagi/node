'use strict';

// Testcases for situations involving fatal errors, like Javascript heap OOM

require('../common');
const assert = require('assert');
const fs = require('fs');
const helper = require('../common/report.js');
const spawnSync = require('child_process').spawnSync;
const tmpdir = require('../common/tmpdir');
const fixtures = require('../common/fixtures');

// Common args that will cause an out-of-memory error for child process.
const ARGS = [
  '--max-old-space-size=20',
  fixtures.path('report-oom'),
];

{
  // Verify that --report-directory is respected when set.
  // Verify that --report-compact is respected when not set.
  tmpdir.refresh();
  const dir = '--report-directory=' + tmpdir.path;
  const args = ['--report-on-fatalerror', dir, ...ARGS];
  const child = spawnSync(process.execPath, args, { });
  assert.notStrictEqual(child.status, 0, 'Process exited unexpectedly');

  const reports = helper.findReports(child.pid, tmpdir.path);
  assert.strictEqual(reports.length, 1);

  const report = reports[0];
  helper.validate(report);
  assert.strictEqual(require(report).header.threadId, null);
  const lines = fs.readFileSync(report, 'utf8').split('\n').length - 1;
  assert(lines > 10);
}
