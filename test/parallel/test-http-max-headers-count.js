'use strict';
require('../common');
const assert = require('assert');
const http = require('http');

let requests = 0;
let responses = 0;

const headers = {};
const N = 2000;
for (let i = 0; i < N; ++i) {
  headers['key' + i] = i;
}

const maxAndExpected = [ // for server
  [50, 50],
  [1500, 1500],
  [0, N + 2] // Host and Connection
];
let max = maxAndExpected[requests][0];
let expected = maxAndExpected[requests][1];

const server = http.createServer(function(req, res) {
  assert.equal(Object.keys(req.headers).length, expected);
  if (++requests < maxAndExpected.length) {
    max = maxAndExpected[requests][0];
    expected = maxAndExpected[requests][1];
    server.maxHeadersCount = max;
  }
  res.writeHead(200, headers);
  res.end();
});
server.maxHeadersCount = max;

server.listen(0, function() {
  const maxAndExpected = [ // for client
    [20, 20],
    [1200, 1200],
    [0, N + 3] // Connection, Date and Transfer-Encoding
  ];
  doRequest();

  function doRequest() {
    const max = maxAndExpected[responses][0];
    const expected = maxAndExpected[responses][1];
    const req = http.request({
      port: server.address().port,
      headers: headers
    }, function(res) {
      assert.equal(Object.keys(res.headers).length, expected);
      res.on('end', function() {
        if (++responses < maxAndExpected.length) {
          doRequest();
        } else {
          server.close();
        }
      });
      res.resume();
    });
    req.maxHeadersCount = max;
    req.end();
  }
});

process.on('exit', function() {
  assert.equal(requests, maxAndExpected.length);
  assert.equal(responses, maxAndExpected.length);
});
