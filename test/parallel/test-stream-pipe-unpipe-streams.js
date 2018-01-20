'use strict';
const common = require('../common');
const assert = require('assert');

const { Readable, Writable } = require('stream');

const source = Readable({ read: () => {} });
const dest1 = Writable({ write: () => {} });
const dest2 = Writable({ write: () => {} });

source.pipe(dest1);
source.pipe(dest2);

dest1.on('unpipe', common.mustCall());
dest2.on('unpipe', common.mustCall());

assert.strictEqual(source._readableState.pipes[0], dest1);
assert.strictEqual(source._readableState.pipes[1], dest2);
assert.strictEqual(source._readableState.pipes.length, 2);

// Should be able to unpipe them in the reverse order that they were piped.

source.unpipe(dest2);

assert.strictEqual(source._readableState.pipes, dest1);
assert.notStrictEqual(source._readableState.pipes, dest2);

dest2.on('unpipe', common.mustNotCall());
source.unpipe(dest2);

source.unpipe(dest1);

assert.strictEqual(source._readableState.pipes, null);

{
  // test `cleanup()` if we unpipe all streams.
  const source = Readable({ read: () => {} });
  const dest1 = Writable({ write: () => {} });
  const dest2 = Writable({ write: () => {} });

  let destCount = 0;
  const srcCheckEventNames = ['end', 'data'];
  const destCheckEventNames = ['close', 'finish', 'drain', 'error', 'unpipe'];

  const checkSrcCleanup = common.mustCall(() => {
    assert.strictEqual(source._readableState.pipes, null);
    assert.strictEqual(source._readableState.pipesCount, 0);
    assert.strictEqual(source._readableState.flowing, false);

    srcCheckEventNames.forEach((eventName) => {
      assert.strictEqual(
        source.listenerCount(eventName), 0,
        `source's '${eventName}' event listeners don't be cleaned up`
      );
    });
  });

  function checkDestCleanup(dest) {
    const currentDestId = ++destCount;
    source.pipe(dest);

    const unpipeChecker = common.mustCall(() => {
      assert.strictEqual(
        dest.listenerCount('unpipe'), 1,
        `destination{${currentDestId}} should have a 'unpipe' event ` +
        'listener which is `unpipeChecker`'
      );
      dest.removeListener('unpipe', unpipeChecker);
      destCheckEventNames.forEach((eventName) => {
        assert.strictEqual(
          dest.listenerCount(eventName), 0,
          `destination{${currentDestId}}'s '${eventName}' event ` +
          "listeners don't be cleaned up"
        );
      });

      if (--destCount === 0)
        checkSrcCleanup();
    });

    dest.on('unpipe', unpipeChecker);
  }

  checkDestCleanup(dest1);
  checkDestCleanup(dest2);
  source.unpipe();
}
