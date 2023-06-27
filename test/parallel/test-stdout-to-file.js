'use strict';
const common = require('../common');
const assert = require('assert');
const path = require('path');
const childProcess = require('child_process');
const fs = require('fs');
const fixtures = require('../common/fixtures');
const tmpdir = require('../common/tmpdir');

const scriptString = fixtures.path('print-chars.js');
const scriptBuffer = fixtures.path('print-chars-from-buffer.js');
const tmpFile = path.join(tmpdir.path, 'stdout.txt');

tmpdir.refresh();

function test(size, useBuffer, cb) {
  const cmd = `"$NODE" "$SCRIPT" ${size} > "$TMP_FILE"`;

  try {
    fs.unlinkSync(tmpFile);
  } catch {
    // Continue regardless of error.
  }

  console.log(`${size} chars to ${tmpFile}...`);

  childProcess.exec(cmd, { env: {
    NODE: process.execPath,
    SCRIPT: useBuffer ? scriptBuffer : scriptString,
    TMP_FILE: tmpFile,
  } }, common.mustSucceed(() => {
    console.log('done!');

    const stat = fs.statSync(tmpFile);

    console.log(`${tmpFile} has ${stat.size} bytes`);

    assert.strictEqual(size, stat.size);
    fs.unlinkSync(tmpFile);

    cb();
  }));
}

test(1024 * 1024, false, common.mustCall(function() {
  console.log('Done printing with string');
  test(1024 * 1024, true, common.mustCall(function() {
    console.log('Done printing with buffer');
  }));
}));
