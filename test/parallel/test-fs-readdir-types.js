'use strict';

const common = require('../common');
const assert = require('assert');
const fs = require('fs');

const tmpdir = require('../common/tmpdir');

const binding = process.binding('fs');

const readdirDir = tmpdir.path;
const files = ['empty', 'files', 'for', 'just', 'testing'];
const constants = process.binding('constants').fs;
const types = {
  isDirectory: constants.UV_DIRENT_DIR,
  isFile: constants.UV_DIRENT_FILE,
  isBlockDevice: constants.UV_DIRENT_BLOCK,
  isCharacterDevice: constants.UV_DIRENT_CHAR,
  isSymbolicLink: constants.UV_DIRENT_LINK,
  isFIFO: constants.UV_DIRENT_FIFO,
  isSocket: constants.UV_DIRENT_SOCKET,
  isUnknown: constants.UV_DIRENT_UNKNOWN
};
const typeMethods = Object.keys(types);

// Make sure tmp directory is clean
tmpdir.refresh();

// Create the necessary files
files.forEach(function(currentFile) {
  fs.closeSync(fs.openSync(`${readdirDir}/${currentFile}`, 'w'));
});


function assertDirents(dirents) {
  assert.strictEqual(files.length, dirents.length);
  for (const [i, dirent] of dirents.entries()) {
    assert(dirent instanceof fs.DirectoryEntry);
    assert.strictEqual(dirent.name, files[i]);
    assert.strictEqual(dirent.isFile(), true);
    assert.strictEqual(dirent.isUnknown(), false);
    assert.strictEqual(dirent.isDirectory(), false);
    assert.strictEqual(dirent.isSocket(), false);
    assert.strictEqual(dirent.isBlockDevice(), false);
    assert.strictEqual(dirent.isCharacterDevice(), false);
    assert.strictEqual(dirent.isFIFO(), false);
    assert.strictEqual(dirent.isSymbolicLink(), false);
  }
}

// Check the readdir Sync version
assertDirents(fs.readdirSync(readdirDir, { withFileTypes: true }));

// Check the readdir async version
fs.readdir(readdirDir, {
  withFileTypes: true
}, common.mustCall((err, dirents) => {
  assert.ifError(err);
  assertDirents(dirents);
}));

// Check for correct types when the binding returns unknowns
const UNKNOWN = constants.UV_DIRENT_UNKNOWN;
const oldReaddir = binding.readdir;
binding.readdir = common.mustCall((path, encoding, types, req, ctx) => {
  if (req) {
    const oldCb = req.oncomplete;
    req.oncomplete = (err, results) => {
      if (err) {
        oldCb(err);
        return;
      }
      results[1] = results[1].map(() => UNKNOWN);
      oldCb(null, results);
    };
    oldReaddir(path, encoding, types, req);
  } else {
    const results = oldReaddir(path, encoding, types, req, ctx);
    results[1] = results[1].map(() => UNKNOWN);
    return results;
  }
}, 2);
assertDirents(fs.readdirSync(readdirDir, { withFileTypes: true }));
fs.readdir(readdirDir, {
  withFileTypes: true
}, common.mustCall((err, dirents) => {
  assert.ifError(err);
  assertDirents(dirents);
}));

// DirectoryEntry types
for (const method of typeMethods) {
  const dirent = new fs.DirectoryEntry('foo', types[method]);
  for (const testMethod of typeMethods) {
    assert.strictEqual(dirent[testMethod](), testMethod === method);
  }
}
