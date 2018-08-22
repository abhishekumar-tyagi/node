'use strict';
const common = require('../common');
if (process.platform !== 'win32')
  common.skip('this test is Windows-specific.');

const fs = require('fs');
const path = require('path');

const tmpdir = require('../common/tmpdir');

// make a path that is more than 260 chars long.
const dirNameLen = Math.max(260 - tmpdir.path.length, 1);
const dirName = path.join(tmpdir.path, 'x'.repeat(dirNameLen));
const fullDirPath = path.resolve(dirName);

const indexFile = path.join(fullDirPath, 'index.js');
const otherFile = path.join(fullDirPath, 'other.js');

tmpdir.refresh();

fs.mkdirSync(fullDirPath);
fs.writeFileSync(indexFile, 'require("./other");');
fs.writeFileSync(otherFile, '');

require(indexFile);
require(otherFile);

tmpdir.refresh();
