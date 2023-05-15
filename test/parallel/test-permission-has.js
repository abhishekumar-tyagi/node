// Flags: --experimental-permission --allow-env=* --allow-fs-read=*
'use strict';

const common = require('../common');

const assert = require('assert');

{
  assert.ok(typeof process.permission.has === 'function');
  assert.throws(() => {
    process.permission.has(null, '');
  }, common.expectsError({
    code: 'ERR_INVALID_ARG_TYPE',
    message: 'The "scope" argument must be of type string. Received null',
  }));
  assert.ok(!process.permission.has('invalid-key'));
  assert.throws(() => {
    process.permission.has('fs', {});
  }, common.expectsError({
    code: 'ERR_INVALID_ARG_TYPE',
    message: 'The "reference" argument must be of type string. Received an instance of Object',
  }));
}

{
  assert.throws(() => {
    process.permission.has('env', '');
  }, common.expectsError({
    code: 'ERR_INVALID_ARG_VALUE'
  }));
  assert.throws(() => {
    process.permission.has('env', '0');
  }, common.expectsError({
    code: 'ERR_INVALID_ARG_VALUE'
  }));
  assert.throws(() => {
    process.permission.has('env', '*');
  }, common.expectsError({
    code: 'ERR_INVALID_ARG_VALUE'
  }));
}
