'use strict';

// Test that fs.copyFile() respects file permissions.
// Ref: https://github.com/nodejs/node/issues/26936

const common = require('../common');

const tmpdir = require('../common/tmpdir');
tmpdir.refresh();

const assert = require('assert');
const fs = require('fs');
const path = require('path');

let n = 0;

function beforeEach() {
  n++;
  const source = path.join(tmpdir.path, `source${n}`);
  const dest = path.join(tmpdir.path, `dest${n}`);
  fs.writeFileSync(source, 'source');
  fs.writeFileSync(dest, 'dest');
  fs.chmodSync(dest, '444');

  const check = (err) => {
    assert.strictEqual(err.code, 'EACCESS');
    assert.strictEqual(fs.readFileSync(dest, 'utf8'), 'dest');
  };

  return { source, dest, check };
}

// Test synchronous API.
{
  const { source, dest, check } = beforeEach();
  assert.throws(() => { fs.copyFileSync(source, dest); }, check);
}

// Test promises API.
{
  const { source, dest, check } = beforeEach();
  assert.throws(async () => { await fs.promises.copyFile(source, dest); },
                check);
}

// Test callback API.
{
  const { source, dest, check } = beforeEach();
  fs.copyFile(source, dest, common.mustCall(check));
}
