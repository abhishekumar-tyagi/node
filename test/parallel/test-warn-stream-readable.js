'use strict';

const common = require('../common');

// _stream_readable is deprecated.

common.expectWarning('DeprecationWarning',
                     'The _stream_readable module is deprecated.', 'DEPXXXX');

require('_stream_readable');
