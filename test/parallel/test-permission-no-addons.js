// Flags: --experimental-permission --allow-fs-read=*
'use strict';

const common = require('../common');
common.skipIfWorker();

const { createRequire } = require('node:module');
const assert = require('node:assert');
const fixtures = require('../common/fixtures');
const loadFixture = createRequire(fixtures.path('node_modules'));
// When a permission is set by cli, the process shouldn't be able
// to require native addons unless --allow-addons is sent
{
  // doesNotThrow
  const msg = loadFixture('pkgexports/no-addons');
  assert.strictEqual(msg, 'not using native addons');
}
