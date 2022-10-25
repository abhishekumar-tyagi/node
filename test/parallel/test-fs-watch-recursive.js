'use strict';

const common = require('../common');

if (common.isIBMi)
  common.skip('IBMi does not support `fs.watch()`');

const assert = require('assert');
const path = require('path');
const fs = require('fs');

const tmpdir = require('../common/tmpdir');

const testDir = tmpdir.path;
const filenameOne = 'watch.txt';

tmpdir.refresh();

const testsubdir = fs.mkdtempSync(testDir + path.sep);
const relativePathOne = path.join(path.basename(testsubdir), filenameOne);
const filepathOne = path.join(testsubdir, filenameOne);
const watcher = fs.watch(testDir, { recursive: true });

let watcherClosed = false;
watcher.on('change', function(event, filename) {
  assert.ok(event === 'change' || event === 'rename');

  // Ignore stale events generated by mkdir and other tests
  if (filename !== relativePathOne)
    return;

  watcher.close();
  watcherClosed = true;
});

setTimeout(common.mustCall(() => {
  fs.writeFileSync(filepathOne, 'world');
}), common.platformTimeout(100));

process.on('exit', function() {
  assert(watcherClosed, 'watcher Object was not closed');
});
