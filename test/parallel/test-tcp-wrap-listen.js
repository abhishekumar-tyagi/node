'use strict';
require('../common');
const assert = require('assert');

const TCP = process.binding('tcp_wrap').TCP;
const WriteWrap = process.binding('stream_wrap').WriteWrap;

const server = new TCP();

const r = server.bind('0.0.0.0', 0);
assert.equal(0, r);
let port = {};
server.getsockname(port);
port = port.port;

server.listen(128);

let sliceCount = 0, eofCount = 0;

let writeCount = 0;
let recvCount = 0;

server.onconnection = function(err, client) {
  assert.equal(0, client.writeQueueSize);
  console.log('got connection');

  function maybeCloseClient() {
    if (client.pendingWrites.length == 0 && client.gotEOF) {
      console.log('close client');
      client.close();
    }
  }

  client.readStart();
  client.pendingWrites = [];
  client.onread = function(err, buffer) {
    if (buffer) {
      assert.ok(buffer.length > 0);

      assert.equal(0, client.writeQueueSize);

      const req = new WriteWrap();
      req.async = false;
      const returnCode = client.writeBuffer(req, buffer);
      assert.equal(returnCode, 0);
      client.pendingWrites.push(req);

      console.log('client.writeQueueSize: ' + client.writeQueueSize);
      // 11 bytes should flush
      assert.equal(0, client.writeQueueSize);

      if (req.async)
        req.oncomplete = done;
      else
        process.nextTick(done.bind(null, 0, client, req));

      function done(status, client_, req_) {
        assert.equal(req, client.pendingWrites.shift());

        // Check parameters.
        assert.equal(0, status);
        assert.equal(client, client_);
        assert.equal(req, req_);

        console.log('client.writeQueueSize: ' + client.writeQueueSize);
        assert.equal(0, client.writeQueueSize);

        writeCount++;
        console.log('write ' + writeCount);
        maybeCloseClient();
      }

      sliceCount++;
    } else {
      console.log('eof');
      client.gotEOF = true;
      server.close();
      eofCount++;
      maybeCloseClient();
    }
  };
};

const net = require('net');

const c = net.createConnection(port);
c.on('connect', function() {
  c.end('hello world');
});

c.setEncoding('utf8');
c.on('data', function(d) {
  assert.equal('hello world', d);
  recvCount++;
});

c.on('close', function() {
  console.error('client closed');
});

process.on('exit', function() {
  assert.equal(1, sliceCount);
  assert.equal(1, eofCount);
  assert.equal(1, writeCount);
  assert.equal(1, recvCount);
});
