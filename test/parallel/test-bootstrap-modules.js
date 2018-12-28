'use strict';

// This list must be computed before we require any modules to
// to eliminate the noise.
const list = process.moduleLoadList.slice();

const common = require('../common');

common.requireFlags('--expose-internals');

const assert = require('assert');

const isMainThread = common.isMainThread;
const kMaxModuleCount = isMainThread ? 63 : 85;

assert(list.length <= kMaxModuleCount,
       `Total length: ${list.length}\n` + list.join('\n')
);
