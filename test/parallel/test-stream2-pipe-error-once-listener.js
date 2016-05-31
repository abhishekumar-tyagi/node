'use strict';
require('../common');
var stream = require('stream');

class Read extends stream.Readable {
  _read(size) {
    this.push('x');
    this.push(null);
  }
}

class Write extends stream.Writable {
  _write(buffer, encoding, cb) {
    this.emit('error', new Error('boom'));
    this.emit('alldone');
  }
}

var read = new Read();
var write = new Write();

write.once('error', function(err) {});
write.once('alldone', function(err) {
  console.log('ok');
});

process.on('exit', function(c) {
  console.error('error thrown even with listener');
});

read.pipe(write);

