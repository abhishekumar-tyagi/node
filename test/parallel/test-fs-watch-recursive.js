'use strict';

const common = require('../common');


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

if (!common.isOSX && !common.isWindows) {
  assert.throws(() => { fs.watch(testDir, { recursive: true }); },
                { code: 'ERR_FEATURE_UNAVAILABLE_ON_PLATFORM' });
  return;
}
const watcher = fs.watch(testDir, { recursive: true });

let watcherClosed = false;
watcher.on('change', function(event, filename) {
  assert.ok(event === 'change' || event === 'rename');

  // Ignore stale events generated by mkdir and other tests
  if (filename !== relativePathOne)
    return;

  if (common.isOSX) {
    clearInterval(interval);
  }
  watcher.close();
  watcherClosed = true;
});

let interval;
if (common.isOSX) {
  interval = setInterval(function() {
    fs.writeFileSync(filepathOne, 'world');
  }, 10);
} else {
  fs.writeFileSync(filepathOne, 'world');
}

process.on('exit', function() {
  assert(watcherClosed, 'watcher Object was not closed');
});
