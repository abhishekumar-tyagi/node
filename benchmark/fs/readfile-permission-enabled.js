// Call fs.readFile with permission system enabled
// over and over again really fast.
// Then see how many times it got called.
'use strict';

const common = require('../common.js');
const fs = require('fs');
const assert = require('assert');

const tmpdir = require('../../test/common/tmpdir');
tmpdir.refresh();
const filename = tmpdir.resolve(`.removeme-benchmark-garbage-${process.pid}`);

const bench = common.createBenchmark(main, {
  duration: [5],
  encoding: ['', 'utf-8'],
  len: [1024, 16 * 1024 * 1024],
  concurrent: [1, 10],
}, {
  flags: [
    '--experimental-permission',
    '--allow-fs-read=*',
    '--allow-fs-write=*',
    '--allow-child-process',
  ],
});

function main({ len, duration, concurrent, encoding }) {
  try {
    fs.unlinkSync(filename);
  } catch {
    // Continue regardless of error.
  }
  let data = Buffer.alloc(len, 'x');
  fs.writeFileSync(filename, data);
  data = null;

  let reads = 0;
  let benchEnded = false;
  bench.start();
  setTimeout(() => {
    benchEnded = true;
    bench.end(reads);

    // This delay is needed because on windows this can cause
    // race condition with afterRead, which makes this benchmark
    // fails to delete the temp file
    setTimeout(() => {
      try {
        fs.unlinkSync(filename);
      } catch {
        // Continue regardless of error.
      }
      process.exit(0);
    }, 10);
  }, duration * 1000);

  function read() {
    fs.readFile(filename, encoding, afterRead);
  }

  function afterRead(er, data) {
    if (er) {
      if (er.code === 'ENOENT') {
        // Only OK if unlinked by the timer from main.
        assert.ok(benchEnded);
        return;
      }
      throw er;
    }

    if (data.length !== len)
      throw new Error('wrong number of bytes returned');

    reads++;
    if (!benchEnded)
      read();
  }

  while (concurrent--) read();
}
