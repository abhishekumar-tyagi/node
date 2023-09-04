'use strict';

const common = require('../common');
const assert = require('node:assert');
const { describe, it } = require('node:test');

const validEnvFilePath = '../fixtures/dotenv/valid.env';
const relativePath = '../fixtures/dotenv/node-options.env';
const simpleEnvFilePath = '../fixtures/dotenv/simple.env';

describe('.env supports edge cases', () => {

  it('should use the last --env-file declaration', async () => {
    const code = `
      require('assert').strictEqual(process.env.BASIC, 'basic');
    `.trim();
    const child = await common.spawnPromisified(
      process.execPath,
      [ `--env-file=${relativePath}`, `--env-file=${validEnvFilePath}`, '--eval', code ],
      { cwd: __dirname },
    );
    assert.strictEqual(child.stderr, '');
    assert.strictEqual(child.code, 0);
  });

  it('should handle non-existent .env file', async () => {
    const code = `
      require('assert').strictEqual(1, 1)
    `.trim();
    const child = await common.spawnPromisified(
      process.execPath,
      [ '--env-file=.env', '--eval', code ],
      { cwd: __dirname },
    );
    assert.strictEqual(child.stderr, '');
    assert.strictEqual(child.code, 0);
  });

  it('should not override existing environment variables', async () => {
    const code = `
      require('assert').strictEqual(process.env.BASIC, 'existing');
    `.trim();
    const child = await common.spawnPromisified(
      process.execPath,
      [ `--env-file=${validEnvFilePath}`, '--eval', code ],
      { cwd: __dirname, env: { BASIC: 'existing' } },
    );
    assert.strictEqual(child.stderr, '');
    assert.strictEqual(child.code, 0);
  });

  it('should not override existing environment variables but introduce new vars', async () => {
    const code = `
      require('assert').strictEqual(process.env.A + "," + process.env.B, '3,2');
    `.trim();
    const child = await common.spawnPromisified(
      process.execPath,
      [ `--env-file=${simpleEnvFilePath}`, '--eval', code ],
      { cwd: __dirname, env: { A: '3' } },
    );
    assert.strictEqual(child.stderr, '');
    assert.strictEqual(child.code, 0);
  });
});
