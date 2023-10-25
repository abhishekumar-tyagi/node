'use strict';

const { spawnPromisified } = require('../common');
const fixtures = require('../common/fixtures.js');
const assert = require('node:assert');
const { execPath } = require('node:process');
const { describe, it } = require('node:test');

describe('Import empty module', { concurrency: true }, () => {
  it(async () => {
    const { code, signal, stdout, stderr } = await spawnPromisified(execPath, [
      '--no-warnings',
      '--eval',
      'import("empty")',
    ], {
      cwd: fixtures.path(),
    });
    assert.strictEqual(code, 1);
    assert.strictEqual(signal, null);
    assert.strictEqual(stdout, '');
    assert.match(stderr, /ERR_INVALID_MODULE/);
  });
});
