// Flags: --inspect=0
'use strict';
const common = require('../common');

common.skipIfInspectorDisabled();

// Assert that even when started with `--inspect=0` workers are assigned
// consecutive (i.e. deterministically predictable) debug ports

const assert = require('assert');
const cluster = require('cluster');

function serialFork() {
  return new Promise((res) => {
    const worker = cluster.fork();
    worker.on('exit', common.mustCall((code, signal) => {
      // code 0 is normal
      // code 12 can happen if inspector could not bind because of a port clash
      if (code !== 0 && code !== 12)
        assert.fail(`code: ${code}, signal: ${signal}`);
      const port = worker.process.spawnargs
        .map((a) => (/=(?:.*:)?(\d{2,5})$/.exec(a) || [])[1])
        .filter((p) => p)
        .pop();
      res(Number(port));
    }));
  });
}

if (cluster.isMaster) {
  Promise.all([serialFork(), serialFork(), serialFork()])
    .then(common.mustCall((ports) => {
      ports.push(process.debugPort);
      ports.sort();
      // 4 = [master, worker1, worker2, worker3].length()
      assert.strictEqual(ports.length, 4);
      assert(ports.every((port) => port > 0));
      assert(ports.every((port) => port < 65536));
      // Ports should be consecutive.
      assert.strictEqual(ports[0] + 1, ports[1]);
      assert.strictEqual(ports[1] + 1, ports[2]);
      assert.strictEqual(ports[2] + 1, ports[3]);
    }))
    .catch(
      (err) => {
        console.error(err);
        process.exit(1);
      });
} else {
  process.exit(0);
}
