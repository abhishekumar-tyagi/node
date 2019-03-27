'use strict';

// Test that fs.copyFile() respects file permissions.
// Ref: https://github.com/nodejs/node/issues/26936

require('../common');

const tmpdir = require('../common/tmpdir');
tmpdir.refresh();

const assert = require('assert');
const fs = require('fs');
const path = require('path');

const source = path.join(tmpdir.path, 'source');
const dest = path.join(tmpdir.path, 'dest');

fs.writeFileSync(source, 'source');
fs.writeFileSync(dest, 'dest');
fs.chmodSync(dest, '444');

fs.copyFile(source, dest, (err) => {
  assert.strictEqual(err.code, 'EACCESS');
  assert.strictEqual(fs.readFileSync(dest, 'utf8'), 'dest');
});
