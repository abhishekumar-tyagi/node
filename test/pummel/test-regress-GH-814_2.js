'use strict';
// Flags: --expose_gc

const common = require('../common');

const fs = require('fs');
const testFileName = require('path').join(common.tmpDir, 'GH-814_test.txt');
const testFD = fs.openSync(testFileName, 'w');
console.error(`${testFileName}\n`);


const tailProc = require('child_process').spawn('tail', ['-f', testFileName]);
tailProc.stdout.on('data', tailCB);

function tailCB(data) {
  PASS = !data.toString().includes('.');

  if (PASS) {
    //console.error('i');
  } else {
    console.error('[FAIL]\n DATA -> ');
    console.error(data);
    console.error('\n');
    throw new Error('Buffers GC test -> FAIL');
  }
}


let PASS = true;
const bufPool = [];
const kBufSize = 16 * 1024 * 1024;
const neverWrittenBuffer = newBuffer(kBufSize, 0x2e); //0x2e === '.'

const timeToQuit = Date.now() + 5e3; //Test should last no more than this.
writer();

function writer() {

  if (PASS) {
    if (Date.now() > timeToQuit) {
      setTimeout(function() {
        process.kill(tailProc.pid);
        console.error('\nBuffers GC test -> PASS (OK)\n');
      }, 555);
    } else {
      fs.write(testFD, newBuffer(kBufSize, 0x61), 0, kBufSize, -1, writerCB);
      global.gc();
      global.gc();
      global.gc();
      global.gc();
      global.gc();
      global.gc();
      const nuBuf = Buffer.allocUnsafe(kBufSize);
      neverWrittenBuffer.copy(nuBuf);
      if (bufPool.push(nuBuf) > 100) {
        bufPool.length = 0;
      }
      process.nextTick(writer);
      //console.error('o');
    }
  }

}

function writerCB(err, written) {
  //console.error('cb.');
  if (err) {
    throw err;
  }
}


// ******************* UTILITIES


function newBuffer(size, value) {
  const buffer = Buffer.allocUnsafe(size);
  while (size--) {
    buffer[size] = value;
  }
  buffer[buffer.length - 1] = 0x0d;
  buffer[buffer.length - 1] = 0x0a;
  return buffer;
}
