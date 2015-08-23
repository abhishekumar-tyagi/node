'use strict';
const assert = require('assert');
const common = require('../common');
const fork = require('child_process').fork;
const net = require('net');
const count = 12;

if (process.argv[2] === 'child') {
  const sockets = [];
  const id = process.argv[3];

  process.on('message', function(m, socket) {
    if (m.cmd === 'new') {
      assert(socket);
      assert(socket instanceof net.Socket, 'should be a net.Socket');
      sockets.push(socket);
      socket.on('end', function() {
        if (!this.closingOnPurpose)
          throw new Error('[c] closing by accident!');
      });
    }

    if (m.cmd === 'close') {
      assert.equal(socket, undefined);
      sockets[m.id].once('close', function() {
        process.send({ id: m.id, status: 'closed' });
      });
      sockets[m.id].destroy();
    }
  });

} else {
  const child = fork(process.argv[1], ['child']);

  child.on('exit', function(code, signal) {
    if (!childKilled)
      throw new Error('child died unexpectedly!');
  });

  const server = net.createServer();
  const sockets = [];
  var sent = 0;

  server.on('connection', function(socket) {
    child.send({ cmd: 'new' }, socket, { track: false });
    sockets.push(socket);

    if (sockets.length === count) {
      closeSockets(0);
    }
  });

  var disconnected = 0;
  const clients = [];
  server.on('listening', function() {
    var j = count, client;
    while (j--) {
      client = net.connect(common.PORT, '127.0.0.1');
      client.id = j;
      client.on('close', function() {
        disconnected += 1;
      });
      clients.push(client);
    }
  });

  var childKilled = false;
  function closeSockets(i) {
    if (i === count) {
      childKilled = true;
      server.close();
      child.kill();
      return;
    }

    sent++;
    child.send({ id: i, cmd: 'close' });
    child.once('message', function(m) {
      assert(m.status === 'closed');
      server.getConnections(function(err, num) {
        closeSockets(i + 1);
      });
    });
  };

  var closeEmitted = false;
  server.on('close', function() {
    closeEmitted = true;
  });

  server.listen(common.PORT, '127.0.0.1');

  process.on('exit', function() {
    assert.equal(sent, count);
    assert.equal(disconnected, count);
    assert.ok(closeEmitted);
    console.log('ok');
  });
}
