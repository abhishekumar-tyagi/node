'use strict';
const common = require('../common');

if (common.isIBMi) {
  common.skip('fs.utimesSync() currently fails on IBM i with Y2K38 values');
}

const tmpdir = require('../common/tmpdir');
tmpdir.refresh();

const assert = require('assert');
const fs = require('fs');

// Check for Y2K38 support. For Windows and AIX, assume it's there. Windows
// doesn't have `touch` and `date -r` which are used in the check for support.
// AIX lacks `date -r`.
if (!common.isWindows && !common.isAIX) {
  const testFilePath = `${tmpdir.path}/y2k38-test`;
  const testFileDate = '204001020304';
  const { spawnSync } = require('child_process');
  const touchResult = spawnSync('touch',
                                ['-t', testFileDate, testFilePath],
                                { encoding: 'utf8' });
  if (touchResult.status !== 0) {
    common.skip('File system appears to lack Y2K38 support (touch failed)');
  }

  const dateResult = spawnSync('date',
                               ['-r', testFilePath, '+%Y%m%d%H%M'],
                               { encoding: 'utf8' });

  assert.strictEqual(dateResult.status, 0);
  if (dateResult.stdout.trim() !== testFileDate) {
    common.skip('File system appears to lack Y2k38 support (date failed)');
  }
}

// Ref: https://github.com/nodejs/node/issues/13255
const path = `${tmpdir.path}/test-utimes-precision`;
fs.writeFileSync(path, '');

const Y2K38_mtime = 2 ** 31;
fs.utimesSync(path, Y2K38_mtime, Y2K38_mtime);
const Y2K38_stats = fs.statSync(path);
assert.strictEqual(Y2K38_stats.mtime.getTime() / 1000, Y2K38_mtime);

if (common.isWindows) {
  // This value would get converted to (double)1713037251359.9998
  const truncate_mtime = 1713037251360;
  fs.utimesSync(path, truncate_mtime / 1000, truncate_mtime / 1000);
  const truncate_stats = fs.statSync(path);
  assert.strictEqual(truncate_stats.mtime.getTime(), truncate_mtime);

  // test Y2K38 for windows
  // This value if treaded as a `signed long` gets converted to -2135622133469.
  // POSIX systems stores timestamps in {long t_sec, long t_usec}.
  // NTFS stores times in nanoseconds in a single `uint64_t`, so when libuv
  // calculates (long)`uv_timespec_t.tv_sec` we get 2's complement.
  const overflow_mtime = 2159345162531;
  fs.utimesSync(path, overflow_mtime / 1000, overflow_mtime / 1000);
  const overflow_stats = fs.statSync(path);
  assert.strictEqual(overflow_stats.mtime.getTime(), overflow_mtime);
}
