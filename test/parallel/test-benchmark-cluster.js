'use strict';

require('../common');

// Minimal test for cluster benchmarks. This makes sure the benchmarks aren't
// horribly broken but nothing more than that.

const assert = require('assert');
const fork = require('child_process').fork;
const path = require('path');

const runjs = path.join(__dirname, '..', '..', 'benchmark', 'run.js');
const argv = ['--set', 'n=1',
              '--set', 'payload=string',
              '--set', 'sendsPerBroadcast=1',
              'cluster'];

const child = fork(runjs, argv);
child.on('exit', (code, signal) => {
  assert.strictEqual(code, 0);
  assert.strictEqual(signal, null);
});
