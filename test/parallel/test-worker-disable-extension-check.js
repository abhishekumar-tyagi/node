// Flags: --no-worker-extension-check

'use strict';
const common = require('../common');
const fixtures = require('../common/fixtures');

const { Worker } = require('worker_threads');

(common.mustCall(() => { new Worker(fixtures.path('foo')); }))();
