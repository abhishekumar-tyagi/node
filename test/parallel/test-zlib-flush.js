'use strict';
var common = require('../common');
var assert = require('assert');
var zlib = require('zlib');
var path = require('path');
var fs = require('fs');

const file = fs.readFileSync(path.resolve(common.fixturesDir, 'person.jpg'));
const chunkSize = 16;
const opts = { level: 0 };
const deflater = zlib.createDeflate(opts);

const chunk = file.slice(0, chunkSize);
const expectedNone = new Buffer([0x78, 0x01]);
const blkhdr = new Buffer([0x00, 0x10, 0x00, 0xef, 0xff]);
const adler32 = new Buffer([0x00, 0x00, 0x00, 0xff, 0xff]);
const expectedFull = Buffer.concat([blkhdr, chunk, adler32]);
let actualNone;
let actualFull;

deflater.write(chunk, function() {
  deflater.flush(zlib.Z_NO_FLUSH, function() {
    actualNone = deflater.read();
    deflater.flush(function() {
      var bufs = [], buf;
      while (buf = deflater.read())
        bufs.push(buf);
      actualFull = Buffer.concat(bufs);
    });
  });
});

process.once('exit', function() {
  assert.deepEqual(actualNone, expectedNone);
  assert.deepEqual(actualFull, expectedFull);
});
