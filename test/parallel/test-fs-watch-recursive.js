'use strict';

const common = require('../common');
const { setTimeout } = require('timers/promises');

if (common.isIBMi)
  common.skip('IBMi does not support `fs.watch()`');

// fs-watch on folders have limited capability in AIX.
// The testcase makes use of folder watching, and causes
// hang. This behavior is documented. Skip this for AIX.

if (common.isAIX)
  common.skip('folder watch capability is limited in AIX.');

const assert = require('assert');
const path = require('path');
const fs = require('fs');
const { pathToFileURL } = require('url');

const tmpdir = require('../common/tmpdir');
const testDir = tmpdir.path;
tmpdir.refresh();

(async () => {
  const filenameOne = 'watch.txt';
  const testsubdir = fs.mkdtempSync(testDir + path.sep);
  const relativePathOne = path.join(path.basename(testsubdir), filenameOne);
  const filepathOne = path.join(testsubdir, filenameOne);
  const watcher = fs.watch(testDir, { recursive: true });

  let watcherClosed = false;
  watcher.on('change', function(event, filename) {
    assert.ok(event === 'change' || event === 'rename');

    if (filename === relativePathOne) {
      watcher.close();
      watcherClosed = true;
    }
  });

  await setTimeout(common.platformTimeout(100));
  fs.writeFileSync(filepathOne, 'world');

  process.once('exit', function() {
    assert(watcherClosed, 'watcher Object was not closed');
  });
})().then(common.mustCall());

(async () => {
  // Add a file to already watching folder

  const testsubdir = fs.mkdtempSync(testDir + path.sep);
  const file = 'file-1.txt';
  const filePath = path.join(testsubdir, file);
  const watcher = fs.watch(testsubdir, { recursive: true });

  let watcherClosed = false;
  watcher.on('change', function(event, filename) {
    assert.ok(event === 'change' || event === 'rename');

    if (filename === file) {
      watcher.close();
      watcherClosed = true;
    }
  });

  await setTimeout(common.platformTimeout(100));
  fs.writeFileSync(filePath, 'world');

  process.once('exit', function() {
    assert(watcherClosed, 'watcher Object was not closed');
  });
})().then(common.mustCall());

(async () => {
  // Add a folder to already watching folder

  const testsubdir = fs.mkdtempSync(testDir + path.sep);
  const file = 'folder-2';
  const filePath = path.join(testsubdir, file);
  const watcher = fs.watch(testsubdir, { recursive: true });

  let watcherClosed = false;
  watcher.on('change', function(event, filename) {
    assert.ok(event === 'change' || event === 'rename');

    if (filename === file) {
      watcher.close();
      watcherClosed = true;
    }
  });

  await setTimeout(common.platformTimeout(100));
  fs.mkdirSync(filePath);

  process.once('exit', function() {
    assert(watcherClosed, 'watcher Object was not closed');
  });
})().then(common.mustCall());

(async () => {
  // Add a file to newly created folder to already watching folder

  const testsubdir = fs.mkdtempSync(testDir + path.sep);
  const file = 'folder-3';
  const filePath = path.join(testsubdir, file);
  const watcher = fs.watch(testsubdir, { recursive: true });
  const childrenFile = 'file-4.txt';
  const childrenAbsolutePath = path.join(filePath, childrenFile);

  let watcherClosed = false;

  watcher.on('change', function(event, filename) {
    assert.ok(event === 'change' || event === 'rename');

    if (filename === path.join(file, childrenFile)) {
      watcher.close();
      watcherClosed = true;
    }
  });

  await setTimeout(common.platformTimeout(100));
  fs.mkdirSync(filePath);
  await setTimeout(common.platformTimeout(100));
  fs.writeFileSync(childrenAbsolutePath, 'world');

  process.once('exit', function() {
    assert(watcherClosed, 'watcher Object was not closed');
  });
})().then(common.mustCall());

(async () => {
  // Add a file to subfolder of a watching folder

  const testsubdir = fs.mkdtempSync(testDir + path.sep);
  const file = 'folder-5';
  const filePath = path.join(testsubdir, file);
  fs.mkdirSync(filePath);

  const subFolder = 'subfolder-6';
  const subfolderPath = path.join(filePath, subFolder);

  fs.mkdirSync(subfolderPath);

  const watcher = fs.watch(testsubdir, { recursive: true });
  const childrenFile = 'file-7.txt';
  const childrenAbsolutePath = path.join(subfolderPath, childrenFile);
  const relativePath = path.join(file, subFolder, childrenFile);

  let watcherClosed = false;

  watcher.on('change', function(event, filename) {
    assert.ok(event === 'change' || event === 'rename');

    if (filename === relativePath) {
      watcher.close();
      watcherClosed = true;
    }
  });

  await setTimeout(common.platformTimeout(100));
  fs.writeFileSync(childrenAbsolutePath, 'world');

  process.once('exit', function() {
    assert(watcherClosed, 'watcher Object was not closed');
  });
})().then(common.mustCall());

(async () => {
  // Add a file to already watching folder, and use URL as the path

  const testsubdir = fs.mkdtempSync(testDir + path.sep);
  const file = 'file-8.txt';
  const filePath = path.join(testsubdir, file);
  const url = pathToFileURL(testsubdir);
  const watcher = fs.watch(url, { recursive: true });

  let watcherClosed = false;
  watcher.on('change', function(event, filename) {
    assert.ok(event === 'change' || event === 'rename');

    if (filename === file) {
      watcher.close();
      watcherClosed = true;
    }
  });

  await setTimeout(common.platformTimeout(100));
  fs.writeFileSync(filePath, 'world');

  process.on('exit', function() {
    assert(watcherClosed, 'watcher Object was not closed');
  });
})().then(common.mustCall());

(async () => {
  // Handle non-boolean values for options.recursive

  if (!common.isWindows && !common.isOSX) {
    assert.throws(() => {
      const testsubdir = fs.mkdtempSync(testDir + path.sep);
      fs.watch(testsubdir, { recursive: '1' });
    }, {
      code: 'ERR_INVALID_ARG_TYPE',
    });
  }
})().then(common.mustCall());
