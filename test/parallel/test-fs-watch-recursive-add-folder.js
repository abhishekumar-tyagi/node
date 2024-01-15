'use strict';

const common = require('../common');

if (common.isIBMi)
  common.skip('IBMi does not support `fs.watch()`');

if (common.isAIX)
  common.skip('folder watch capability is limited in AIX.');

const assert = require('assert');
const path = require('path');
const fs = require('fs');

const tmpdir = require('../common/tmpdir');
const testDir = tmpdir.path;
tmpdir.refresh();

// Add a folder to already watching folder

const rootDirectory = fs.mkdtempSync(testDir + path.sep);
const testDirectory = path.join(rootDirectory, 'test-2');
fs.mkdirSync(testDirectory);

const testFile = path.join(testDirectory, 'folder-2');

const watcher = fs.watch(testDirectory, { recursive: true });
let watcherClosed = false;
watcher.on('change', function(event, filename) {
  assert.strictEqual(event, 'rename');

  if (filename === path.basename(testFile)) {
    watcher.close();
    watcherClosed = true;
  }
});

fs.mkdirSync(testFile);

process.once('exit', function() {
  assert(watcherClosed, 'watcher Object was not closed');
});
