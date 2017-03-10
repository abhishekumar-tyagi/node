'use strict';

const common = require('../common');
const assert = require('assert');
const net = require('net');
const fs = require('fs');
const uv = process.binding('uv');
const TCP = process.binding('tcp_wrap').TCP;
const Pipe = process.binding('pipe_wrap').Pipe;

common.refreshTmpDir();

function closeServer() {
  return common.mustCall(function() {
    this.close();
  });
}

// server.listen(pipe) creates a new pipe wrap,
// so server.close() doesn't actually unlink this existing pipe.
// It needs to be unlinked separately via handle.close()
function closePipeServer(handle) {
  return common.mustCall(function() {
    this.close();
    handle.close();
  });
}

let counter = 0;

// Avoid conflict with listen-path
function randomPipePath() {
  return common.PIPE + '-listen-handle-' + (counter++);
}

function randomHandle(type) {
  let handle, errno, handleName;
  if (type === 'tcp') {
    handle = new TCP();
    errno = handle.bind('0.0.0.0', 0);
    handleName = 'arbitrary tcp port';
  } else {
    const path = randomPipePath();
    handle = new Pipe();
    errno = handle.bind(path);
    handleName = `pipe ${path}`;
  }

  if (errno < 0) {  // uv.errname requires err < 0
    assert(errno >= 0, `unable to bind ${handleName}: ${uv.errname(errno)}`);
  }
  // err >= 0 but fd = -1, should not happen
  assert(handle.fd !== -1, `Bound ${handleName} has fd -1 and errno ${errno}`);
  return handle;
}

// Not a public API, used by child_process
{
  // Test listen(tcp)
  net.createServer()
    .listen(randomHandle('tcp'))
    .on('listening', closeServer());
  // Test listen(tcp, cb)
  net.createServer()
    .listen(randomHandle('tcp'), closeServer());
}

function randomPipes(number) {
  const arr = [];
  for (let i = 0; i < number; ++i) {
    arr.push(randomHandle('pipe'));
  }
  return arr;
}

// Not a public API, used by child_process
{
  const handles = randomPipes(2);  // generate pipes in advance
  // Test listen(pipe)
  net.createServer()
    .listen(handles[0])
    .on('listening', closePipeServer(handles[0]));
  // Test listen(pipe, cb)
  net.createServer()
    .listen(handles[1], closePipeServer(handles[1]));
}

{
  // Test listen({handle: tcp}, cb)
  net.createServer()
    .listen({handle: randomHandle('tcp')}, closeServer());
  // Test listen({handle: tcp})
  net.createServer()
    .listen({handle: randomHandle('tcp')})
    .on('listening', closeServer());
  // Test listen({_handle: tcp}, cb)
  net.createServer()
    .listen({_handle: randomHandle('tcp')}, closeServer());
  // Test listen({_handle: tcp})
  net.createServer()
    .listen({_handle: randomHandle('tcp')})
    .on('listening', closeServer());
  // Test listen({fd: tcp.fd}, cb)
  net.createServer()
    .listen({fd: randomHandle('tcp').fd}, closeServer());
  // Test listen({fd: tcp.fd})
  net.createServer()
    .listen({fd: randomHandle('tcp').fd})
    .on('listening', closeServer());
}

{
  const handles = randomPipes(6);  // generate pipes in advance
  // Test listen({handle: pipe}, cb)
  net.createServer()
    .listen({handle: handles[0]}, closePipeServer(handles[0]));
  // Test listen({handle: pipe})
  net.createServer()
    .listen({handle: handles[1]})
    .on('listening', closePipeServer(handles[1]));
  // Test listen({_handle: pipe}, cb)
  net.createServer()
    .listen({_handle: handles[2]}, closePipeServer(handles[2]));
  // Test listen({_handle: pipe})
  net.createServer()
    .listen({_handle: handles[3]})
    .on('listening', closePipeServer(handles[3]));
  // Test listen({fd: pipe.fd}, cb)
  net.createServer()
    .listen({fd: handles[4].fd}, closePipeServer(handles[4]));
  // Test listen({fd: pipe.fd})
  net.createServer()
    .listen({fd: handles[5].fd})
    .on('listening', closePipeServer(handles[5]));
}

{
  // Test invalid fd
  const fd = fs.openSync(__filename, 'r');
  net.createServer()
    .listen({fd: fd}, common.mustNotCall())
    .on('error', common.mustCall(function(err) {
      assert.strictEqual(err + '', 'Error: listen EINVAL');
      this.close();
    }));
}
