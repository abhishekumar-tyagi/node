'use strict';
const common = require('../common');
const assert = require('assert');
const http = require('http');

const server = http.createServer((req, res) => res.end());

common.refreshTmpDir();

server.listen(common.PIPE, () =>
  asyncLoop(makeKeepAliveRequest, 10, () =>
    server.getConnections(common.mustCall((err, conns) => {
      assert.ifError(err);
      assert.strictEqual(1, conns);
      server.close();
    }))
  )
);

function asyncLoop(fn, times, cb) {
  fn(function handler() {
    if (--times) {
      fn(handler);
    } else {
      cb();
    }
  });
}
function makeKeepAliveRequest(cb) {
  http.get({
    socketPath: common.PIPE,
    headers: {connection: 'keep-alive'}
  }, (res) => res.on('data', () => {}).on('error', assert.fail).on('end', cb));
}
