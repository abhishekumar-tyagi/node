'use strict';

const common = require('../common');
common.requireFlags('--pending-deprecation');

process.on('warning', common.mustNotCall('A warning should not be emitted'));

// With the --pending-deprecation flag, the deprecation warning for
// new Buffer() should not be emitted when Uint8Array methods are called.

Buffer.from('abc').map((i) => i);
Buffer.from('abc').filter((i) => i);
Buffer.from('abc').slice(1, 2);
Buffer.of(0, 1);
