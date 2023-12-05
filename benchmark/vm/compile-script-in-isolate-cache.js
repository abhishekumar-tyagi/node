'use strict';

// This benchmarks compiling scripts that hit the in-isolate compilation
// cache (by having the same source).
const common = require('../common.js');
const fs = require('fs');
const vm = require('vm');
const fixtures = require('../../test/common/fixtures.js');
const scriptPath = fixtures.path('snapshot', 'typescript.js');

const bench = common.createBenchmark(main, {
  type: ['with-dynamic-import-callback', 'without-dynamic-import-callback'],
  n: [100],
});

const scriptSource = fs.readFileSync(scriptPath, 'utf8');

function main({ n, type }) {
  let script;
  bench.start();
  const options = {};
  switch (type) {
    case 'with-dynamic-import-callback':
      // Use a dummy callback for now until we really need to benchmark it.
      options.importModuleDynamically = async () => {};
      break;
    case 'without-dynamic-import-callback':
      break;
  }
  for (let i = 0; i < n; i++) {
    script = new vm.Script(scriptSource, options);
  }
  bench.end(n);
  script.runInThisContext();
}
