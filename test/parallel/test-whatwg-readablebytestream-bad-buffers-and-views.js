'use strict';

const common = require('../common');
const assert = require('node:assert');

{
  // ReadableStream with byte source: respondWithNewView() throws if the
  // supplied view's buffer has a different length (in the closed state)
  const stream = new ReadableStream({
    pull: common.mustCall(async (c) => {
      const view = new Uint8Array(new ArrayBuffer(10), 0, 0);

      c.close();

      assert.throws(() => {
        c.byobRequest.respondWithNewView(view);
      }, RangeError);
    }),
    type: 'bytes',
  });

  const reader = stream.getReader({ mode: 'byob' });
  reader.read(new Uint8Array([4, 5, 6]));
}

{
  // ReadableStream with byte source: respondWithNewView() throws if the
  // supplied view's buffer has been detached (in the closed state)
  const stream = new ReadableStream({
    pull: common.mustCall((c) => {
      c.close();

      // Detach it by reading into it
      const view = new Uint8Array([1, 2, 3]);
      reader.read(view);

      assert.throws(() => {
        c.byobRequest.respondWithNewView(view);
      }, TypeError);
    }),
    type: 'bytes',
  });

  const reader = stream.getReader({ mode: 'byob' });
  reader.read(new Uint8Array([4, 5, 6]));
}
