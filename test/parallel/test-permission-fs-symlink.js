// Flags: --experimental-permission --allow-fs-read=* --allow-fs-write=* --allow-child-process
'use strict';

const common = require('../common');
common.skipIfWorker();

const fixtures = require('../common/fixtures');
if (!common.canCreateSymLink())
  common.skip('insufficient privileges');
if (!common.hasCrypto)
  common.skip('no crypto');

const assert = require('assert');
const fs = require('fs');
const { spawnSync } = require('child_process');
const path = require('path');
const tmpdir = require('../common/tmpdir');

const file = fixtures.path('permission', 'fs-symlink.js');
const commonPathWildcard = path.join(__filename, '../../common*');
const blockedFile = fixtures.path('permission', 'deny', 'protected-file.md');
const blockedFolder = path.join(tmpdir.path, 'subdirectory');
const symlinkFromBlockedFile = path.join(tmpdir.path, 'example-symlink.md');

{
  tmpdir.refresh();
  fs.mkdirSync(blockedFolder);
}

{
  // Symlink previously created
  fs.symlinkSync(blockedFile, symlinkFromBlockedFile);
}

{
  const { status, stderr } = spawnSync(
    process.execPath,
    [
      '--experimental-permission',
      `--allow-fs-read=${file},${commonPathWildcard},${symlinkFromBlockedFile}`,
      `--allow-fs-write=${symlinkFromBlockedFile}`,
      file,
    ],
    {
      env: {
        ...process.env,
        BLOCKEDFOLDER: blockedFolder,
        BLOCKEDFILE: blockedFile,
        EXISTINGSYMLINK: symlinkFromBlockedFile,
      },
    }
  );
  assert.strictEqual(status, 0, stderr.toString());
}

{
  tmpdir.refresh();
}
