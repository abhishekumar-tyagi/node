'use strict';
const common = require('../common');
const assert = require('assert');
const net = require('net');
const http = require('http');

let requestReceived = 0;

const server = http.createServer(function(req, res) {
  const id = ++requestReceived;
  const enoughToDrain = req.connection.writableHighWaterMark;
  const body = 'x'.repeat(enoughToDrain);

  if (id === 1) {
    // Case of needParse = false
    req.connection.once('pause', common.mustCall(() => {
      assert(req.connection._paused, '_paused must be true because it exceeds' +
                                     'highWaterMark by second request');
    }));
    res.write(body);
  } else {
    // Case of needParse = true
    const resume = req.connection.parser.resume.bind(req.connection.parser);
    req.connection.parser.resume = common.mustCall((...args) => {
      const paused = req.connection._paused;
      assert(!paused, '_paused must be false because it become false by ' +
                      'socketOnDrain when outgoingData falls below ' +
                      'highWaterMark');
      return resume(...args);
    });
    assert(!res.write(body), 'res.write must be fail because it will exceed ' +
                             'highWaterMark on this call');
  }
  res.end();
}).on('listening', () => {
  const c = net.createConnection(server.address().port, () => {
    c.write('GET / HTTP/1.1\r\n\r\n');
    c.write('GET / HTTP/1.1\r\n\r\n');
    c.end();
  });

  c.on('data', () => {});
  c.on('end', () => {
    server.close();
  });
})
.listen(0);
