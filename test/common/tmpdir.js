/* eslint-disable node-core/require-common-first, node-core/required-modules */
'use strict';

const fs = require('fs');
const path = require('path');
const { isMainThread } = require('worker_threads');

function rmSync(pathname) {
  let err = null;
  const maxRetries = 10;

  for (let retryNumber = 0; retryNumber < maxRetries; ++retryNumber) {
    err = null;

    try {
      fs.rmSync(pathname, { maxRetries: 3, recursive: true, force: true });
    } catch (_err) {
      err = _err;
    }

    if (err === null) {
      return;
    }

    const errPath = err.path;
    const errCode = err.code;

    if (errCode === 'EACCES' || errCode === 'EPERM') {
      const surroundingDir = path.join(errPath, '..');

      try { fs.chmodSync(surroundingDir, 0o777); } catch {}
      try { fs.chmodSync(errPath, 0o777); } catch {}
    }
  }

  if (err === null) {
    return;
  }

  throw err;
}

const testRoot = process.env.NODE_TEST_DIR ?
  fs.realpathSync(process.env.NODE_TEST_DIR) : path.resolve(__dirname, '..');

// Using a `.` prefixed name, which is the convention for "hidden" on POSIX,
// gets tools to ignore it by default or by simple rules, especially eslint.
const tmpdirName = '.tmp.' +
  (process.env.TEST_SERIAL_ID || process.env.TEST_THREAD_ID || '0');
const tmpPath = path.join(testRoot, tmpdirName);

let firstRefresh = true;
function refresh() {
  rmSync(this.path);
  fs.mkdirSync(this.path);

  if (firstRefresh) {
    firstRefresh = false;
    // Clean only when a test uses refresh. This allows for child processes to
    // use the tmpdir and only the parent will clean on exit.
    process.on('exit', onexit);
  }
}

function logErrorInfo(err) {
  console.error('Can\'t clean tmpdir:', tmpPath);

  const files = fs.readdirSync(tmpPath);
  console.error('Files blocking:', files);

  if (files.some((f) => f.startsWith('.nfs'))) {
    // Warn about NFS "silly rename"
    console.error('Note: ".nfs*" might be files that were open and ' +
                  'unlinked but not closed.');
    console.error('See http://nfs.sourceforge.net/#faq_d2 for details.');
  }

  // Additionally logging err, just in case throwing err gets overshadowed by
  // an error from a test failure.
  console.error(err);

  throw err;
}

function onexit() {
  // Change directory to avoid possible EBUSY
  if (isMainThread)
    process.chdir(testRoot);

  try {
    rmSync(tmpPath);
  } catch (err) {
    logErrorInfo(err);
  }
}

module.exports = {
  path: tmpPath,
  refresh
};
