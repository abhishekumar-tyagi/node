'use strict';
const common = require('../common');
const assert = require('assert');
const path = require('path');
const fs = require('fs');

const realModuleSymlinkPath = path.join(common.fixturesDir,
  '/module-require-real-path/node_modules/_real-module');

const localRealModulePath = path.join(common.fixturesDir,
  '/module-require-real-path/real-module');

// module-require-real-path exports its require function. That way we can test
// how modules get resolved **from** its.
const _require = require(
  path.join(common.fixturesDir, '/module-require-real-path')
).internalRequire;

process.on('exit', function() {
  fs.unlinkSync(realModuleSymlinkPath);
});

fs.symlinkSync('../real-module', realModuleSymlinkPath);

assert.equal(_require('./real-module'), _require('real-module'));
assert.equal(_require('real-module').dirname, localRealModulePath);

// When required directly with the _-prefix, resolve to path of symlink.
assert.notEqual(_require('./real-module'), _require('_real-module'));
assert.equal(_require('_real-module').dirname, realModuleSymlinkPath);
