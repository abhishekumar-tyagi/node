'use strict';

require('../common');
const assert = require('assert');

const {
  executionAsyncResource,
  executionAsyncId,
  createHook,
} = require('async_hooks');
const http = require('http');

const hooked = {};
createHook({
  init(asyncId, type, triggerAsyncId, resource) {
    hooked[asyncId] = resource;
  }
}).enable();

const agent = new http.Agent({
  maxSockets: 1
});

const server = http.createServer((req, res) => {
  res.end('ok');
});

server.listen(0, () => {
  assert.strictEqual(executionAsyncResource(), hooked[executionAsyncId()]);

  http.get({ agent, port: server.address().port }, () => {
    assert.strictEqual(executionAsyncResource(), hooked[executionAsyncId()]);
    server.close();
    agent.destroy();
  });
});
