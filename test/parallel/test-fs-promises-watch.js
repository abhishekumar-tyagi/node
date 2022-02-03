'use strict';
const common = require('../common');

if (common.isIBMi)
  common.skip('IBMi does not support `fs.watch()`');

const { watch } = require('fs/promises');
const fs = require('fs');
const assert = require('assert');
const { join } = require('path');
const tmpdir = require('../common/tmpdir');

class WatchTestCase {
  constructor(shouldInclude, dirName, fileName, field) {
    this.dirName = dirName;
    this.fileName = fileName;
    this.field = field;
    this.shouldSkip = !shouldInclude;
  }
  get dirPath() { return join(tmpdir.path, this.dirName); }
  get filePath() { return join(this.dirPath, this.fileName); }
}

const kCases = [
  // Watch on a directory should callback with a filename on supported systems
  new WatchTestCase(
    common.isLinux || common.isOSX || common.isWindows || common.isAIX,
    'watch1',
    'foo',
    'filePath'
  ),
  // Watch on a file should callback with a filename on supported systems
  new WatchTestCase(
    common.isLinux || common.isOSX || common.isWindows,
    'watch2',
    'bar',
    'dirPath'
  ),
];

tmpdir.refresh();

for (const testCase of kCases) {
  if (testCase.shouldSkip) continue;
  fs.mkdirSync(testCase.dirPath);
  // Long content so it's actually flushed.
  const content1 = Date.now() + testCase.fileName.toLowerCase().repeat(1e4);
  fs.writeFileSync(testCase.filePath, content1);

  let interval;
  async function test() {
    const watcher = watch(testCase[testCase.field]);
    for await (const { eventType, filename } of watcher) {
      clearInterval(interval);
      assert.strictEqual(['rename', 'change'].includes(eventType), true);
      assert.strictEqual(filename, testCase.fileName);
      break;
    }

    // Waiting on it again is a non-op
    for await (const {} of watcher) {
      assert.fail('should not run');
    }
  }

  // Long content so it's actually flushed. toUpperCase so there's real change.
  const content2 = Date.now() + testCase.fileName.toUpperCase().repeat(1e4);
  interval = setInterval(() => {
    fs.writeFileSync(testCase.filePath, '');
    fs.writeFileSync(testCase.filePath, content2);
  }, 100);

  test().then(common.mustCall());
}

assert.rejects(
  async () => { for await (const {} of watch(1)) {} },
  { code: 'ERR_INVALID_ARG_TYPE' });

assert.rejects(
  async () => { for await (const {} of watch(__filename, 1)) {} },
  { code: 'ERR_INVALID_ARG_TYPE' });

assert.rejects(
  async () => { for await (const {} of watch('', { persistent: 1 })) {} },
  { code: 'ERR_INVALID_ARG_TYPE' });

assert.rejects(
  async () => { for await (const {} of watch('', { recursive: 1 })) {} },
  { code: 'ERR_INVALID_ARG_TYPE' });

assert.rejects(
  async () => { for await (const {} of watch('', { encoding: 1 })) {} },
  { code: 'ERR_INVALID_ARG_VALUE' });

assert.rejects(
  async () => { for await (const {} of watch('', { signal: 1 })) {} },
  { code: 'ERR_INVALID_ARG_TYPE' });

(async () => {
  const ac = new AbortController();
  const { signal } = ac;
  setImmediate(() => ac.abort());
  try {
    for await (const {} of watch(__filename, { signal })) {}
  } catch (err) {
    assert.strictEqual(err.name, 'AbortError');
  }
})().then(common.mustCall());
