'use strict';

const common = require('../common');
const http = require('http');

const server = http.createServer(handle);

function handle(req, res) {
  res.on('error', common.mustCall((err) => {
    common.expectsError({
      code: 'ERR_STREAM_WRITE_AFTER_END',
      type: Error
    })(err);
    server.close();
  }));

  res.write('hello');
  res.end();

  setImmediate(common.mustCall(() => {
    res.end('world');
  }));
}

server.listen(0, common.mustCall(() => {
  http.get(`http://localhost:${server.address().port}`);
}));
