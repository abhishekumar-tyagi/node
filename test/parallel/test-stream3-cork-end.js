'use strict';
require('../common');
const assert = require('assert');
const stream = require('stream');
const Writable = stream.Writable;

// Test the buffering behaviour of Writable streams.
//
// The call to cork() triggers storing chunks which are flushed
// on calling end() and the stream subsequently ended.
//
// node version target: 0.12

const expectedChunks = ['please', 'buffer', 'me', 'kindly'];
var inputChunks = expectedChunks.slice(0);
var seenChunks = [];
var seenEnd = false;

var w = new Writable();
// lets arrange to store the chunks
w._write = function(chunk, encoding, cb) {
  // stream end event is not seen before the last write
  assert.ok(!seenEnd);
  // default encoding given none was specified
  assert.equal(encoding, 'buffer');

  seenChunks.push(chunk);
  cb();
};
// lets record the stream end event
w.on('finish', () => {
  seenEnd = true;
});

function writeChunks(remainingChunks, callback) {
  var writeChunk = remainingChunks.shift();
  var writeState;

  if (writeChunk) {
    setImmediate(() => {
      writeState = w.write(writeChunk);
      // we were not told to stop writing
      assert.ok(writeState);

      writeChunks(remainingChunks, callback);
    });
  } else {
    callback();
  }
}

// do an initial write
w.write('stuff');
// the write was immediate
assert.equal(seenChunks.length, 1);
// reset the seen chunks
seenChunks = [];

// trigger stream buffering
w.cork();

// write the bufferedChunks
writeChunks(inputChunks, () => {
  // should not have seen anything yet
  assert.equal(seenChunks.length, 0);

  // trigger flush and ending the stream
  w.end();

  // stream should not ended in current tick
  assert.ok(!seenEnd);

  // buffered bytes should be seen in current tick
  assert.equal(seenChunks.length, 4);

  // did the chunks match
  for (var i = 0, l = expectedChunks.length; i < l; i++) {
    var seen = seenChunks[i];
    // there was a chunk
    assert.ok(seen);

    var expected = new Buffer(expectedChunks[i]);
    // it was what we expected
    assert.ok(seen.equals(expected));
  }

  setImmediate(() => {
    // stream should have ended in next tick
    assert.ok(seenEnd);
  });
});
