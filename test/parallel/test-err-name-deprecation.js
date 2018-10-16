'use strict';
const common = require('../common');

// Flags: --pending-deprecation

common.expectWarning(
  'DeprecationWarning',
  'Directly calling process.binding(\'uv\').errname(<val>) is being ' +
  'deprecated. Please make sure to use util.getSystemErrorName() instead.',
  'DEP0XXX'
);

process.binding('uv').errname(-1);
