// Flags: --expose-internals
'use strict';

const common = require('../common');
const { it, describe } = require('node:test');
const assert = require('node:assert');

const fixtures = require('../common/fixtures');
const envSuffix = common.isWindows ? '-windows' : '';

describe('node run [command]', () => {
  it('should emit experimental warning', async () => {
    const child = await common.spawnPromisified(
      process.execPath,
      [ '--run', 'test'],
      { cwd: __dirname },
    );
    assert.match(child.stderr, /ExperimentalWarning: Task runner is an experimental feature and might change at any time/);
    assert.match(child.stdout, /Can't read package\.json/);
    assert.strictEqual(child.code, 1);
  });

  it('returns error on non-existent file', async () => {
    const child = await common.spawnPromisified(
      process.execPath,
      [ '--no-warnings', '--run', 'test'],
      { cwd: __dirname },
    );
    assert.match(child.stdout, /Can't read package\.json/);
    assert.strictEqual(child.stderr, '');
    assert.strictEqual(child.code, 1);
  });

  it('runs a valid command', async () => {
    // Run a script that just log `no test specified`
    const child = await common.spawnPromisified(
      process.execPath,
      [ '--run', 'test', '--no-warnings'],
      { cwd: fixtures.path('run-script') },
    );
    assert.match(child.stdout, /Error: no test specified/);
    assert.strictEqual(child.code, 1);
  });

  it('adds node_modules/.bin to path', async () => {
    const child = await common.spawnPromisified(
      process.execPath,
      [ '--no-warnings', '--run', `ada${envSuffix}`],
      { cwd: fixtures.path('run-script') },
    );
    assert.match(child.stdout, /06062023/);
    assert.strictEqual(child.stderr, '');
    assert.strictEqual(child.code, 0);
  });

  it('appends positional arguments', async () => {
    const child = await common.spawnPromisified(
      process.execPath,
      [ '--no-warnings', '--run', `positional-args${envSuffix}`, '--', '--help "hello world test"'],
      { cwd: fixtures.path('run-script') },
    );
    assert.match(child.stdout, /--help "hello world test"/);
    assert.strictEqual(child.stderr, '');
    assert.strictEqual(child.code, 0);
  });

  it('should support having --env-file cli flag', async () => {
    const child = await common.spawnPromisified(
      process.execPath,
      [ '--no-warnings', `--env-file=${fixtures.path('run-script/.env')}`, '--run', `custom-env${envSuffix}`],
      { cwd: fixtures.path('run-script') },
    );
    assert.match(child.stdout, /hello world/);
    assert.strictEqual(child.stderr, '');
    assert.strictEqual(child.code, 0);
  });

  it('should properly escape shell', async () => {
    const { escapeShell } = require('internal/shell');

    const expectations = [
      ['', '\'\''],
      ['test', 'test'],
      ['test words', '\'test words\''],
      ['$1', '\'$1\''],
      ['"$1"', '\'"$1"\''],
      ['\'$1\'', '\\\'\'$1\'\\\''],
      ['\\$1', '\'\\$1\''],
      ['--arg="$1"', '\'--arg="$1"\''],
      ['--arg=node exec -c "$1"', '\'--arg=node exec -c "$1"\''],
      ['--arg=node exec -c \'$1\'', '\'--arg=node exec -c \'\\\'\'$1\'\\\''],
      ['\'--arg=node exec -c "$1"\'', '\\\'\'--arg=node exec -c "$1"\'\\\''],
    ];

    for (const [input, expectation] of expectations) {
      assert.strictEqual(escapeShell(input), expectation);
    }
  });
});
