'use strict';
var common = require('../common');
var assert = require('assert');
var http = require('http');

var serverGotConnect = false;
var clientGotConnect = false;

var server = http.createServer(function(req, res) {
  assert(false);
});
server.on('connect', function(req, socket, firstBodyChunk) {
  assert.equal(req.method, 'CONNECT');
  assert.equal(req.url, 'google.com:443');
  console.error('Server got CONNECT request');
  serverGotConnect = true;

  // It is legal for the server to send some data intended for the client
  // along with the CONNECT response
  socket.write('HTTP/1.1 200 Connection established\r\n\r\nHead');

  var data = firstBodyChunk.toString();
  socket.on('data', function(buf) {
    data += buf.toString();
  });
  socket.on('end', function() {
    socket.end(data);
  });
});
server.listen(common.PORT, function() {
  var req = http.request({
    port: common.PORT,
    method: 'CONNECT',
    path: 'google.com:443'
  }, function(res) {
    assert(false);
  });

  var clientRequestClosed = false;
  req.on('close', function() {
    clientRequestClosed = true;
  });

  req.on('connect', function(res, socket, firstBodyChunk) {
    console.error('Client got CONNECT request');
    clientGotConnect = true;

    // Make sure this request got removed from the pool.
    var name = 'localhost:' + common.PORT;
    assert(!http.globalAgent.sockets.hasOwnProperty(name));
    assert(!http.globalAgent.requests.hasOwnProperty(name));

    // Make sure this socket has detached.
    assert(!socket.ondata);
    assert(!socket.onend);
    assert.equal(socket.listeners('connect').length, 0);
    assert.equal(socket.listeners('data').length, 0);

    // the stream.Duplex onend listener
    // allow 0 here, so that i can run the same test on streams1 impl
    assert(socket.listeners('end').length <= 1);

    assert.equal(socket.listeners('free').length, 0);
    assert.equal(socket.listeners('close').length, 0);
    assert.equal(socket.listeners('error').length, 0);
    assert.equal(socket.listeners('agentRemove').length, 0);

    var data = firstBodyChunk.toString();

    // test that the firstBodyChunk was not parsed as HTTP
    assert.equal(data, 'Head');

    socket.on('data', function(buf) {
      data += buf.toString();
    });
    socket.on('end', function() {
      assert.equal(data, 'HeadRequestEnd');
      assert(clientRequestClosed);
      server.close();
    });
    socket.end('End');
  });

  req.end('Request');
});

process.on('exit', function() {
  assert.ok(serverGotConnect);
  assert.ok(clientGotConnect);
});
