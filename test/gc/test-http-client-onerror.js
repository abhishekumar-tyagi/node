'use strict';
// just like test/gc/http-client.js,
// but with an on('error') handler that does nothing.

function serverHandler(req, res) {
  req.resume();
  res.writeHead(200, {'Content-Type': 'text/plain'});
  res.end('Hello World\n');
}

const http = require('http');
const weak = require('weak');
require('../common');
const assert = require('assert');
const todo = 500;
let done = 0;
let count = 0;
let countGC = 0;

console.log('We should do ' + todo + ' requests');

const server = http.createServer(serverHandler);
server.listen(0, runTest);

function getall() {
  if (count >= todo)
    return;

  (function() {
    function cb(res) {
      res.resume();
      done += 1;
      statusLater();
    }
    function onerror(er) {
      throw er;
    }

    const req = http.get({
      hostname: 'localhost',
      pathname: '/',
      port: server.address().port
    }, cb).on('error', onerror);

    count++;
    weak(req, afterGC);
  })();

  setImmediate(getall);
}

function runTest() {
  for (let i = 0; i < 10; i++)
    getall();
}

function afterGC() {
  countGC++;
}

let timer;
function statusLater() {
  gc();
  if (timer) clearTimeout(timer);
  timer = setTimeout(status, 1);
}

function status() {
  gc();
  console.log('Done: %d/%d', done, todo);
  console.log('Collected: %d/%d', countGC, count);
  if (done === todo) {
    console.log('All should be collected now.');
    assert.strictEqual(count, countGC);
    process.exit(0);
  }
}

