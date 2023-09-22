'use strict';
const common = require('../common');
const assert = require('assert');
const http = require('http');
const Countdown = require('../common/countdown');

const expectedSuccesses = [undefined, null, 'GET', 'post'];
const expectedFails = [-1, 1, 0, {}, true, false, [], Symbol()];

const countdown =
  new Countdown(expectedSuccesses.length,
                common.mustCall(() => server.close()));

const server = http.createServer(common.mustCall((req, res) => {
  res.end();
  countdown.dec();
}, expectedSuccesses.length));

server.listen(0, common.mustCall(() => {
  for (let i = 0; i < expectedFails.length; i++) {
    const method = expectedFails[i];
    assert.throws(() => {
      http.request({ method, path: '/' }, common.mustNotCall());
    }, {
      code: 'ERR_INVALID_ARG_TYPE',
      name: 'TypeError',
      message: 'The "options.method" property must be of type string.' +
               common.invalidArgTypeHelper(method)
    });
  }

  for (let i = 0; i < expectedSuccesses.length; i++) {
    const method = expectedSuccesses[i];
    http.request({ method, port: server.address().port }).end();
  }
}));
