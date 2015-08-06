'use strict';
const assert = require('assert');
const path = require('path');
const common = require('../common');

if (common.isWindows) {
  var file = path.join(common.fixturesDir, 'a.js');
  var resolvedFile = path.resolve(file);

  assert.equal('\\\\?\\' + resolvedFile, path._makeLong(file));
  assert.equal('\\\\?\\' + resolvedFile, path._makeLong('\\\\?\\' + file));
  assert.equal('\\\\?\\UNC\\someserver\\someshare\\somefile',
               path._makeLong('\\\\someserver\\someshare\\somefile'));
  assert.equal('\\\\?\\UNC\\someserver\\someshare\\somefile',
               path._makeLong('\\\\?\\UNC\\someserver\\someshare\\somefile'));
  assert.equal('\\\\.\\pipe\\somepipe',
               path._makeLong('\\\\.\\pipe\\somepipe'));
}

assert.equal(path._makeLong(null), null);
assert.equal(path._makeLong(100), 100);
assert.equal(path._makeLong(path), path);
assert.equal(path._makeLong(false), false);
assert.equal(path._makeLong(true), true);
