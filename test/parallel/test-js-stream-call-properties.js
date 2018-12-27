'use strict';

const common = require('../common');

common.requireFlags(['--expose-internals']);

const util = require('util');
const { internalBinding } = require('internal/test/binding');
const { JSStream } = internalBinding('js_stream');

// Testing if will abort when properties are printed.
util.inspect(new JSStream());
