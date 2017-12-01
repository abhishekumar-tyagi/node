'use strict';
const common = require('../common');

if (!common.hasCrypto)
  common.skip('missing crypto');

const crypto = require('crypto');
const notExistsEngineName = 'xxx';

common.expectsError(
  () => crypto.setEngine(true),
  {
    code: 'ERR_INVALID_ARG_TYPE',
    type: TypeError,
    message: 'The "id" argument must be of type string'
  });

common.expectsError(
  () => crypto.setEngine('/path/to/engine', 'notANumber'),
  {
    code: 'ERR_INVALID_ARG_TYPE',
    type: TypeError,
    message: 'The "flags" argument must be of type number'
  });

common.expectsError(
  () => crypto.setEngine(notExistsEngineName),
  {
    code: 'ERR_CRYPTO_ENGINE_UNKNOWN',
    type: Error,
    message: `Engine "${notExistsEngineName}" was not found`
  });

common.expectsError(
  () => crypto.setEngine(notExistsEngineName,
                         crypto.constants.ENGINE_METHOD_RSA),
  {
    code: 'ERR_CRYPTO_ENGINE_UNKNOWN',
    type: Error,
    message: `Engine "${notExistsEngineName}" was not found`
  });
