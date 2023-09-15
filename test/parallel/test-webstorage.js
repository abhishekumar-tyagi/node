'use strict';
const common = require('../common');
const tmpdir = require('../common/tmpdir');
const assert = require('node:assert');
const { join } = require('node:path');
const { readdir } = require('node:fs/promises');
const { test } = require('node:test');
const { spawnPromisified } = common;
let cnt = 0;

tmpdir.refresh();

function nextLocalStorage() {
  return join(tmpdir.path, `${++cnt}.localstorage`);
}

test('disabled without --experimental-webstorage', async () => {
  for (const api of ['Storage', 'localStorage', 'sessionStorage']) {
    const cp = await spawnPromisified(process.execPath, ['-e', api]);

    assert.strictEqual(cp.code, 1);
    assert.strictEqual(cp.signal, null);
    assert.strictEqual(cp.stdout, '');
    assert(cp.stderr.includes(`ReferenceError: ${api} is not defined`));
  }
});

test('emits a warning when used', async () => {
  for (const api of ['Storage', 'localStorage', 'sessionStorage']) {
    const cp = await spawnPromisified(process.execPath, [
      '--experimental-webstorage',
      '--localstorage-file', nextLocalStorage(),
      '-e', api,
    ]);

    assert.strictEqual(cp.code, 0);
    assert.strictEqual(cp.signal, null);
    assert.strictEqual(cp.stdout, '');
    assert.match(cp.stderr, /ExperimentalWarning: Web storage/);
  }
});

test('Storage instances cannot be created in userland', async () => {
  const cp = await spawnPromisified(process.execPath, [
    '--experimental-webstorage', '-e', 'new globalThis.Storage()',
  ]);

  assert.strictEqual(cp.code, 1);
  assert.strictEqual(cp.signal, null);
  assert.strictEqual(cp.stdout, '');
  assert.match(cp.stderr, /Error: Illegal constructor/);
});

test('sessionStorage is not persisted', async () => {
  let cp = await spawnPromisified(process.execPath, [
    '--experimental-webstorage', '-pe', 'sessionStorage.foo = "barbaz"',
  ]);
  assert.strictEqual(cp.code, 0);
  assert.match(cp.stdout, /barbaz/);

  cp = await spawnPromisified(process.execPath, [
    '--experimental-webstorage', '-pe', 'sessionStorage.foo',
  ]);
  assert.strictEqual(cp.code, 0);
  assert.match(cp.stdout, /undefined/);
  assert.strictEqual((await readdir(tmpdir.path)).length, 0);
});

test('localStorage throws without --localstorage-file ', async () => {
  const cp = await spawnPromisified(process.execPath, [
    '--experimental-webstorage',
    '-pe', 'localStorage === global.localStorage',
  ]);
  assert.strictEqual(cp.code, 1);
  assert.strictEqual(cp.signal, null);
  assert.strictEqual(cp.stdout, '');
  assert.match(cp.stderr, /The argument '--localstorage-file' is an invalid localStorage location/);
});

test('localStorage is not persisted if it is unused', async () => {
  const cp = await spawnPromisified(process.execPath, [
    '--experimental-webstorage',
    '--localstorage-file', nextLocalStorage(),
    '-pe', 'localStorage === global.localStorage',
  ]);
  assert.strictEqual(cp.code, 0);
  assert.match(cp.stdout, /true/);
  assert.strictEqual((await readdir(tmpdir.path)).length, 0);
});

test('localStorage is persisted if it is used', async () => {
  const localStorageFile = nextLocalStorage();
  let cp = await spawnPromisified(process.execPath, [
    '--experimental-webstorage',
    '--localstorage-file', localStorageFile,
    '-pe', 'localStorage.foo = "barbaz"',
  ]);
  assert.strictEqual(cp.code, 0);
  assert.match(cp.stdout, /barbaz/);
  const entries = await readdir(tmpdir.path);
  assert.strictEqual(entries.length, 1);
  assert.match(entries[0], /\d+\.localstorage/);

  cp = await spawnPromisified(process.execPath, [
    '--experimental-webstorage',
    '--localstorage-file', localStorageFile,
    '-pe', 'localStorage.foo',
  ]);
  assert.strictEqual(cp.code, 0);
  assert.match(cp.stdout, /barbaz/);
});
