'use strict';

const common = require('../common');
if (!common.hasCrypto)
  common.skip('missing crypto');

const assert = require('assert');
const fs = require('fs');
const path = require('path');
const tls = require('tls');

const tick = require('./tick');
const initHooks = require('./init-hooks');
const { checkInvocations } = require('./hook-checks');

const hooks = initHooks();
hooks.enable();

//
// Creating server and listening on port
//
const server = tls
  .createServer({
    cert: fs.readFileSync(path.join(common.fixturesDir, 'test_cert.pem')),
    key: fs.readFileSync(path.join(common.fixturesDir, 'test_key.pem'))
  })
  .on('listening', common.mustCall(onlistening))
  .on('secureConnection', common.mustCall(onsecureConnection))
  .listen(common.PORT);

let svr, client;
function onlistening() {
  //
  // Creating client and connecting it to server
  //
  tls
    .connect(common.PORT, { rejectUnauthorized: false })
    .on('secureConnect', common.mustCall(onsecureConnect));

  const as = hooks.activitiesOfTypes('TLSWRAP');
  assert.strictEqual(as.length, 1);
  svr = as[0];

  assert.strictEqual(svr.type, 'TLSWRAP');
  assert.strictEqual(typeof svr.uid, 'number');
  assert.strictEqual(typeof svr.triggerAsyncId, 'number');
  checkInvocations(svr, { init: 1 }, 'server: when client connecting');
}

function onsecureConnection() {
  //
  // Server received client connection
  //
  const as = hooks.activitiesOfTypes('TLSWRAP');
  assert.strictEqual(as.length, 2);
  client = as[1];
  assert.strictEqual(client.type, 'TLSWRAP');
  assert.strictEqual(typeof client.uid, 'number');
  assert.strictEqual(typeof client.triggerAsyncId, 'number');

  // TODO(thlorenz) which callback did the server wrap execute that already
  // finished as well?
  checkInvocations(svr, { init: 1, before: 1, after: 1 },
                   'server: when server has secure connection');

  checkInvocations(client, { init: 1, before: 2, after: 1 },
                   'client: when server has secure connection');
}

function onsecureConnect() {
  //
  // Client connected to server
  //
  checkInvocations(svr, { init: 1, before: 2, after: 1 },
                   'server: when client connected');
  checkInvocations(client, { init: 1, before: 2, after: 2 },
                   'client: when client connected');

  //
  // Destroying client socket
  //
  this.destroy();
  checkInvocations(svr, { init: 1, before: 2, after: 1 },
                   'server: when destroying client');
  checkInvocations(client, { init: 1, before: 2, after: 2 },
                   'client: when destroying client');

  tick(5, tick1);
  function tick1() {
    checkInvocations(svr, { init: 1, before: 2, after: 2 },
                     'server: when client destroyed');
    // TODO: why is client not destroyed here even after 5 ticks?
    // or could it be that it isn't actually destroyed until
    // the server is closed?
    checkInvocations(client, { init: 1, before: 3, after: 3 },
                     'client: when client destroyed');
    //
    // Closing server
    //
    server.close(common.mustCall(onserverClosed));
    // No changes to invocations until server actually closed below
    checkInvocations(svr, { init: 1, before: 2, after: 2 },
                     'server: when closing server');
    checkInvocations(client, { init: 1, before: 3, after: 3 },
                     'client: when closing server');
  }
}

function onserverClosed() {
  //
  // Server closed
  //
  tick(1E4, common.mustCall(() => {
    checkInvocations(svr, { init: 1, before: 2, after: 2 },
                     'server: when server closed');
    checkInvocations(client, { init: 1, before: 3, after: 3 },
                     'client: when server closed');
  }));
}

process.on('exit', onexit);

function onexit() {
  hooks.disable();
  hooks.sanityCheck('TLSWRAP');

  checkInvocations(svr, { init: 1, before: 2, after: 2 },
                   'server: when process exits');
  checkInvocations(client, { init: 1, before: 3, after: 3 },
                   'client: when process exits');
}
