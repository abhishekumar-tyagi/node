'use strict';

const common = require('../common');
const assert = require('assert');
const http = require('http');
const helloWorld = 'Hello World!';
const helloAgainLater = 'Hello again later!';

const server = http.createServer((req, res) => {
  res.writeHead(200, {
    'Content-Length': '' + (helloWorld.length + helloAgainLater.length)
  });
  res.write(helloWorld);

  // we need to make sure the data is flushed
  setTimeout(() => {
    res.end(helloAgainLater);
  }, common.platformTimeout(10));
}).listen(0, function() {
  const opts = {
    hostname: 'localhost',
    port: server.address().port,
    path: '/'
  };

  const expectedData = [helloWorld, helloAgainLater];
  const expectedRead = [helloWorld, null, helloAgainLater, null];

  const req = http.request(opts, (res) => {
    res.on('error', common.mustNotCall);

    res.on('readable', common.mustCall(() => {
      let data;

      do {
        data = res.read();
        assert.strictEqual(data, expectedRead.shift());
      } while (data !== null);
    }, 2));

    res.setEncoding('utf8');
    res.on('data', common.mustCall((data) => {
      assert.strictEqual(data, expectedData.shift());
    }, 2));

    res.on('end', common.mustCall(() => {
      server.close();
    }));
  });

  req.end();
});
