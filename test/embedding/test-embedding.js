'use strict';
const common = require('../common');
const fixtures = require('../common/fixtures');
const tmpdir = require('../common/tmpdir');
const assert = require('assert');
const {
  spawnSyncAndExit,
  spawnSyncAndExitWithoutError,
} = require('../common/child_process');
const path = require('path');
const fs = require('fs');

tmpdir.refresh();
common.allowGlobals(global.require);
common.allowGlobals(global.embedVars);

function resolveBuiltBinary(binary) {
  if (common.isWindows) {
    binary += '.exe';
  }
  return path.join(path.dirname(process.execPath), binary);
}

const binary = resolveBuiltBinary('embedtest');

spawnSyncAndExitWithoutError(
  binary,
  ['console.log(42)'],
  {
    trim: true,
    stdout: '42',
  });

spawnSyncAndExitWithoutError(
  binary,
  ['console.log(embedVars.nön_ascıı)'],
  {
    trim: true,
    stdout: '🏳️‍🌈',
  });

spawnSyncAndExit(
  binary,
  ['throw new Error()'],
  {
    status: 1,
    signal: null,
  });

spawnSyncAndExit(
  binary,
  ['require("lib/internal/test/binding")'],
  {
    status: 1,
    signal: null,
  });

spawnSyncAndExit(
  binary,
  ['process.exitCode = 8'],
  {
    status: 8,
    signal: null,
  });

const fixturePath = JSON.stringify(fixtures.path('exit.js'));
spawnSyncAndExit(
  binary,
  [`require(${fixturePath})`, 92],
  {
    status: 92,
    signal: null,
  });

function getReadFileCodeForPath(path) {
  return `(require("fs").readFileSync(${JSON.stringify(path)}, "utf8"))`;
}

// Basic snapshot support
for (const extraSnapshotArgs of [[], ['--embedder-snapshot-as-file']]) {
  // readSync + eval since snapshots don't support userland require() (yet)
  const snapshotFixture = fixtures.path('snapshot', 'echo-args.js');
  const blobPath = tmpdir.resolve('embedder-snapshot.blob');
  const buildSnapshotExecArgs = [
    `eval(${getReadFileCodeForPath(snapshotFixture)})`, 'arg1', 'arg2',
  ];
  const embedTestBuildArgs = [
    '--embedder-snapshot-blob', blobPath, '--embedder-snapshot-create',
    ...extraSnapshotArgs,
  ];
  const buildSnapshotArgs = [
    ...buildSnapshotExecArgs,
    ...embedTestBuildArgs,
  ];

  const runSnapshotExecArgs = [
    'arg3', 'arg4',
  ];
  const embedTestRunArgs = [
    '--embedder-snapshot-blob', blobPath,
    ...extraSnapshotArgs,
  ];
  const runSnapshotArgs = [
    ...runSnapshotExecArgs,
    ...embedTestRunArgs,
  ];

  fs.rmSync(blobPath, { force: true });
  spawnSyncAndExitWithoutError(
    binary,
    [ '--', ...buildSnapshotArgs ],
    { cwd: tmpdir.path },
    {});
  spawnSyncAndExitWithoutError(
    binary,
    [ '--', ...runSnapshotArgs ],
    { cwd: tmpdir.path },
    {
      stdout(output) {
        assert.deepStrictEqual(JSON.parse(output), {
          originalArgv: [binary, '__node_anonymous_main', ...buildSnapshotExecArgs],
          currentArgv: [binary, ...runSnapshotExecArgs],
        });
        return true;
      },
    });
}

// Create workers and vm contexts after deserialization
{
  const snapshotFixture = fixtures.path('snapshot', 'create-worker-and-vm.js');
  const blobPath = tmpdir.resolve('embedder-snapshot.blob');
  const buildSnapshotArgs = [
    `eval(${getReadFileCodeForPath(snapshotFixture)})`,
    '--embedder-snapshot-blob', blobPath, '--embedder-snapshot-create',
  ];
  const runEmbeddedArgs = [
    '--embedder-snapshot-blob', blobPath,
  ];

  fs.rmSync(blobPath, { force: true });

  spawnSyncAndExitWithoutError(
    binary,
    [ '--', ...buildSnapshotArgs ],
    { cwd: tmpdir.path },
    {});
  spawnSyncAndExitWithoutError(
    binary,
    [ '--', ...runEmbeddedArgs ],
    { cwd: tmpdir.path },
    {});
}
