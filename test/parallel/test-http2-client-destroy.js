// Flags: --expose-http2
'use strict';

const common = require('../common');
if (!common.hasCrypto)
  common.skip('missing crypto');
const assert = require('assert');
const h2 = require('http2');

{
  const server = h2.createServer();
  server.listen(
    0,
    common.mustCall(() => {
      const destroyCallbacks = [
        (client) => client.destroy(),
        (client) => client.socket.destroy()
      ];

      let remaining = destroyCallbacks.length;

      destroyCallbacks.forEach((destroyCallback) => {
        const client = h2.connect(`http://localhost:${server.address().port}`);
        client.on(
          'connect',
          common.mustCall(() => {
            const socket = client.socket;

            assert(client.socket, 'client session has associated socket');
            assert(
              !client.destroyed,
              'client has not been destroyed before destroy is called'
            );
            assert(
              !socket.destroyed,
              'socket has not been destroyed before destroy is called'
            );

            // Ensure that 'close' event is emitted
            client.on('close', common.mustCall());

            destroyCallback(client);

            assert(
              !client.socket,
              'client.socket undefined after destroy is called'
            );

            // Must must be closed
            client.on(
              'close',
              common.mustCall(() => {
                assert(client.destroyed);
              })
            );

            // socket will close on process.nextTick
            socket.on(
              'close',
              common.mustCall(() => {
                assert(socket.destroyed);
              })
            );

            if (--remaining === 0) {
              server.close();
            }
          })
        );
      });
    })
  );
}

// test destroy before connect
{
  const server = h2.createServer();
  server.listen(
    0,
    common.mustCall(() => {
      const client = h2.connect(`http://localhost:${server.address().port}`);

      const req = client.request({ ':path': '/' });
      client.destroy();

      req.on('response', common.mustNotCall());
      req.resume();
      req.on(
        'end',
        common.mustCall(() => {
          server.close();
        })
      );
      req.end();
    })
  );
}

// test destroy before request
{
  const server = h2.createServer();
  server.listen(
    0,
    common.mustCall(() => {
      const client = h2.connect(`http://localhost:${server.address().port}`);
      client.destroy();

      assert.throws(
        () => client.request({ ':path': '/' }),
        common.expectsError({
          code: 'ERR_HTTP2_INVALID_SESSION',
          message: 'The session has been destroyed'
        })
      );

      server.close();
    })
  );
}

// test destroy before goaway
{
  const server = h2.createServer();
  server.on(
    'stream',
    common.mustCall((stream) => {
      stream.on('error', common.mustCall());
      stream.session.shutdown();
    })
  );
  server.listen(
    0,
    common.mustCall(() => {
      const client = h2.connect(`http://localhost:${server.address().port}`);

      client.on(
        'goaway',
        common.mustCall(() => {
          // We ought to be able to destroy the client in here without an error
          server.close();
          client.destroy();
        })
      );

      client.request();
    })
  );
}
