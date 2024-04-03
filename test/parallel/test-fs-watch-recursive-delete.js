'use strict';

const common = require('../common');
const tmpdir = require('../common/tmpdir');
const fs = require('fs');
const assert = require('node:assert');

tmpdir.refresh();

fs.mkdirSync(tmpdir.resolve('./parent/child'), { recursive: true });

fs.writeFileSync(tmpdir.resolve('./parent/child/test.tmp'), 'test');

const watcher = fs.watch(tmpdir.resolve('./parent'), { recursive: true }, common.mustCall((eventType, filename) => {
  // We are only checking for the filename to avoid having Windows, Linux and Mac specific assertions
  assert.strictEqual(filename.indexOf('test.tmp') >= 0, true);
  watcher.close();
}));

fs.rmSync(tmpdir.resolve('./parent/child/test.tmp'));
