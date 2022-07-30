import { spawnPromisified } from '../common/index.mjs';
import fixtures from '../common/fixtures.js';
import assert from 'node:assert';
import { execPath } from 'node:process';
import { describe, it } from 'node:test';

const cjsEntry = fixtures.path('es-modules', 'cjs-file.cjs');
const cjsImport = fixtures.fileURL('es-modules', 'cjs-file.cjs');
const mjsEntry = fixtures.path('es-modules', 'mjs-file.mjs');
const mjsImport = fixtures.fileURL('es-modules', 'mjs-file.mjs');


describe('import modules using --import', { concurrency: true }, () => {
  it('should import when using --eval', async () => {
    const { code, signal, stderr, stdout } = await spawnPromisified(
      execPath,
      [
        '--import', mjsImport,
        '--eval', 'console.log("log")',
      ]
    );

    assert.strictEqual(stderr, '');
    assert.match(stdout, /^\.mjs file\r?\nlog\r?\n$/);
    assert.strictEqual(code, 0);
    assert.strictEqual(signal, null);
  });

  it('should import when using --eval and --input-type=module', async () => {
    const { code, signal, stderr, stdout } = await spawnPromisified(
      execPath,
      [
        '--import', mjsImport,
        '--input-type', 'module',
        '--eval', 'console.log("log")',
      ]
    );

    assert.strictEqual(stderr, '');
    assert.match(stdout, /^\.mjs file\r?\nlog\r?\n$/);
    assert.strictEqual(code, 0);
    assert.strictEqual(signal, null);
  });

  it('should import when main entrypoint is a cjs file', async () => {
    const { code, signal, stderr, stdout } = await spawnPromisified(
      execPath,
      [
        '--import', mjsImport,
        cjsEntry,
      ]
    );

    assert.strictEqual(stderr, '');
    assert.match(stdout, /^\.mjs file\r?\n\.cjs file\r?\n$/);
    assert.strictEqual(code, 0);
    assert.strictEqual(signal, null);
  });

  it('should import mjs when entrypoint is a cjs file', async () => {
    const { code, signal, stderr, stdout } = await spawnPromisified(
      execPath,
      [
        '--import', mjsImport,
        cjsEntry,
      ]
    );

    assert.strictEqual(stderr, '');
    assert.match(stdout, /^\.mjs file\r?\n\.cjs file\r?\n$/);
    assert.strictEqual(code, 0);
    assert.strictEqual(signal, null);
  });

  it('should import cjs when entrypoint is a mjs file', async () => {
    const { code, signal, stderr, stdout } = await spawnPromisified(
      execPath,
      [
        '--import', cjsImport,
        mjsEntry,
      ]
    );

    assert.strictEqual(stderr, '');
    assert.match(stdout, /^\.cjs file\r?\n\.mjs file\r?\n$/);
    assert.strictEqual(code, 0);
    assert.strictEqual(signal, null);
  });

  it('should import mjs when entrypoint is a cjs file', async () => {
    const { code, signal, stderr, stdout } = await spawnPromisified(
      execPath,
      [
        '--import', mjsImport,
        cjsEntry,
      ]
    );

    assert.strictEqual(stderr, '');
    assert.match(stdout, /^\.mjs file\r?\n\.cjs file\r?\n$/);
    assert.strictEqual(code, 0);
    assert.strictEqual(signal, null);
  });

  it('should de-duplicate redundant `--import`s', async () => {
    const { code, signal, stderr, stdout } = await spawnPromisified(
      execPath,
      [
        '--import', mjsImport,
        '--import', mjsImport,
        '--import', mjsImport,
        cjsEntry,
      ]
    );

    assert.strictEqual(stderr, '');
    assert.match(stdout, /^\.mjs file\r?\n\.cjs file\r?\n$/);
    assert.strictEqual(code, 0);
    assert.strictEqual(signal, null);
  });

  it('should import when running --check', async () => {
    const { code, signal, stderr, stdout } = await spawnPromisified(
      execPath,
      [
        '--import', mjsImport,
        '--check',
        cjsEntry,
      ]
    );

    assert.strictEqual(stderr, '');
    assert.match(stdout, /^\.mjs file\r?\n$/);
    assert.strictEqual(code, 0);
    assert.strictEqual(signal, null);
  });

  it('should import when running --check fails', async () => {
    const { code, signal, stderr, stdout } = await spawnPromisified(
      execPath,
      [
        '--import', mjsImport,
        '--no-warnings',
        '--check',
        fixtures.path('es-modules', 'invalid-cjs.js'),
      ]
    );

    assert.match(stderr, /SyntaxError: Unexpected token 'export'/);
    assert.match(stdout, /^\.mjs file\r?\n$/);
    assert.strictEqual(code, 1);
    assert.strictEqual(signal, null);
  });

  it('should import a module with top level await', async () => {
    const { code, signal, stderr, stdout } = await spawnPromisified(
      execPath,
      [
        '--import', fixtures.fileURL('es-modules', 'esm-top-level-await.mjs'),
        fixtures.path('es-modules', 'print-3.mjs'),
      ]
    );

    assert.strictEqual(stderr, '');
    assert.match(stdout, /^1\r?\n2\r?\n3\r?\n$/);
    assert.strictEqual(code, 0);
    assert.strictEqual(signal, null);
  });
});
