// Flags: --experimental-permission --allow-env --allow-fs-read=* --allow-child-process
'use strict';

require('../common');
const { spawnSync } = require('node:child_process');
const { debuglog } = require('node:util');
const { strictEqual, deepStrictEqual } = require('node:assert');
const { describe, it } = require('node:test');

const debug = debuglog('test');

function runTest(args, options = {}) {
  const { status, stdout, stderr } = spawnSync(
    process.execPath,
    ['--experimental-permission', '--allow-fs-read=*', ...args],
    options
  );
  debug('status:', status);
  if (status) debug('stdout:', stdout.toString().split('\n'));
  if (status) debug('stderr:', stderr.toString().split('\n'));
  return { status, stdout, stderr };
}

describe('permission: has "env" with the reference', () => {
  const code = `
  const has = (...args) => console.log(process.permission.has(...args));
  has('env', 'HOME');
  has('env', 'PORT');
  has('env', 'DEBUG');
  `;

  for (const flag of ['--allow-env', '--allow-env-name']) {
    it(`has: ${flag}=HOME`, () => {
      const { status, stdout } = runTest([`${flag}=HOME`, '-e', code]);
      strictEqual(status, 0);
      deepStrictEqual(stdout.toString().split('\n').slice(0, -1), [
        'true',
        'false',
        'false',
      ]);
    });

    it(`has: ${flag}=HOME,PORT`, () => {
      const { status, stdout } = runTest([`${flag}=HOME,PORT`, '-e', code]);

      strictEqual(status, 0);
      deepStrictEqual(stdout.toString().split('\n').slice(0, -1), [
        'true',
        'true',
        'false',
      ]);
    });
  }

  it('has: --allow-env', () => {
    const { status, stdout } = runTest(['--allow-env', '-e', code]);

    strictEqual(status, 0);
    deepStrictEqual(stdout.toString().split('\n').slice(0, -1), [
      'true',
      'true',
      'true',
    ]);
  });
});

describe('permission: has "env" with no reference', () => {
  it('initial state', () => {
    const { status } = runTest([
      '-e',
      'require("assert").strictEqual(process.permission.has("env"), false);',
    ]);
    strictEqual(status, 0);
  });

  it('has no reference: --allow-env', () => {
    const { status } = runTest([
      '--allow-env',
      '-e',
      'require("assert").strictEqual(process.permission.has("env"), true);',
    ]);
    strictEqual(status, 0);
  });

  for (const flag of ['--allow-env', '--allow-env-name']) {
    it(`has no reference: ${flag}=HOME`, () => {
      const { status } = runTest([
        `${flag}=HOME`,
        '-e',
        'require("assert").strictEqual(process.permission.has("env"), false);',
      ]);
      strictEqual(status, 0);
    });
  }
});

describe('permission: has "env" reference', () => {
  for (const flag of ['--allow-env', '--allow-env-name']) {
    it(`reference is case-sensitive: ${flag}=FoO`, () => {
      const { status, stdout } = runTest([
        `${flag}=FoO`,
        '-e',
        `
        console.log(process.permission.has('env', 'FOO'));
        console.log(process.permission.has('env', 'foo'));
        console.log(process.permission.has('env', 'FoO'));
        `,
      ]);
      strictEqual(status, 0);
      deepStrictEqual(stdout.toString().split('\n').slice(0, -1), [
        'false',
        'false',
        'true',
      ]);
    });
  }
});
