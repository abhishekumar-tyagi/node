'use strict';
const common = require('../common');
const fs = require('fs');
const cp = require('child_process');
const path = require('path');

common.refreshTmpDir();

const LOG_FILE = path.join(common.tmpDir, 'tick-processor.log');
const RETRY_TIMEOUT = 750;

function runTest(test) {
  const proc = cp.spawn(process.execPath, [
    '--no_logfile_per_isolate',
    '--logfile=-',
    '--prof',
    '-pe', test.code
  ], {
    stdio: [ null, 'pipe', 'inherit' ]
  });

  let ticks = '';
  proc.stdout.on('data', chunk => ticks += chunk);

  // Try to match after timeout
  setTimeout(() => {
    match(test.pattern, proc, () => ticks);
  }, RETRY_TIMEOUT);
}

function match(pattern, parent, ticks) {
  // Store current ticks log
  fs.writeFileSync(LOG_FILE, ticks());

  const proc = cp.spawn(process.execPath, [
    '--prof-process',
    '--call-graph-size=10',
    LOG_FILE
  ], {
    stdio: [ null, 'pipe', 'inherit' ]
  });

  let out = '';
  proc.stdout.on('data', chunk => out += chunk);
  proc.stdout.on('end', () => {
    // Retry after timeout
    if (!pattern.test(out))
      return setTimeout(() => match(pattern, parent, ticks), RETRY_TIMEOUT);

    parent.kill('SIGTERM');

    fs.unlinkSync(LOG_FILE);
  });
}

exports.runTest = runTest;
