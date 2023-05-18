// Flags: --experimental-permission --allow-env=* --allow-fs-read=* --allow-child-process
'use strict';

require('../common');
const { spawnSync } = require('node:child_process');
const { debuglog } = require('node:util');
const { strictEqual } = require('node:assert');
const { describe, it } = require('node:test');

const debug = debuglog('test');

function runTest(args, options = {}) {
  const { status, stdout, stderr } = spawnSync(
    process.execPath,
    ['--experimental-permission', '--allow-fs-read=*', ...args],
    options
  );
  debug('status:', status);
  if (status) debug('stdout:', stdout?.toString().split('\n'));
  if (status) debug('stderr:', stderr?.toString().split('\n'));
  return { status, stdout, stderr };
}

describe('permission: enumerate', () => {
  const error = JSON.stringify({
    code: 'ERR_ACCESS_DENIED',
    permission: 'Environment',
  });

  it('enumerate with *,-UNDEFINED', () => {
    const { status } = runTest([
      '--allow-env=*,-UNDEFINED',
      '-e',
      `
      const { doesNotThrow } = require('node:assert');
      doesNotThrow(() => {
        Object.keys(process.env);
      });
      `,
    ]);
    strictEqual(status, 0);
  });

  it('enumerate with *,-DEFINED', () => {
    const { status } = runTest(
      [
        '--allow-env=*,-DEFINED',
        '-e',
        `
        const { throws } = require('node:assert');
        throws(() => {
          Object.keys(process.env);
        }, ${error});
        `,
      ],
      {
        env: {
          ...process.env,
          DEFINED: 0,
        },
      }
    );
    strictEqual(status, 0);
  });
});

describe('permission: structuredClone', () => {
  const error = JSON.stringify({
    code: 'ERR_ACCESS_DENIED',
    permission: 'Environment',
  });

  it('structuredClone process.env with *,-UNDEFINED', () => {
    const { status } = runTest([
      '--allow-env=*,-UNDEFINED',
      '-e',
      `
      const { doesNotThrow } = require('node:assert');
      doesNotThrow(() => {
        structuredClone(process.env);
      });
      `,
    ]);
    strictEqual(status, 0);
  });

  it('structuredClone process.env with *,-DEFINED', () => {
    const { status } = runTest(
      [
        '--allow-env=*,-DEFINED',
        '-e',
        `
        const { throws } = require('node:assert');
        throws(() => {
          structuredClone(process.env);
        }, ${error});
        `,
      ],
      {
        env: {
          ...process.env,
          DEFINED: 0,
        },
      }
    );
    strictEqual(status, 0);
  });
});
