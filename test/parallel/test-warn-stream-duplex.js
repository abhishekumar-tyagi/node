'use strict';

const common = require('../common');

// _stream_duplex is deprecated.

common.expectWarning('DeprecationWarning',
                     'The _stream_duplex module is deprecated.', 'DEPXXXX');

require('_stream_duplex');
