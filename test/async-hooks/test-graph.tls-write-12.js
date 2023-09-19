'use strict';

const common = require('../common');
if (!common.hasCrypto)
  common.skip('missing crypto');

const tls = require('tls');

tls.DEFAULT_MAX_VERSION = 'TLSv1.2';

require('./test-graph.tls-write.js');
