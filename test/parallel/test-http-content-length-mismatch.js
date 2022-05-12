'use strict';
const common = require('../common');
const http = require('node:http');

// Simple test to check if body size is less  than the content length set is

const server = http.createServer(common.mustCall((req, res) => {
  res.setHeader('content-length', 110);
  // This line should not throw any error
  res.write('hello');
  res.end();
}));

server.listen(0, common.mustCall(() => {
  http.get({
    port: server.address().port
  }, common.mustCall((res) => {
    server.close();

    res.resume();
  }));
}));
