'use strict';
const common = require('../common');
const EventEmitter = require('events');
const util = require('util');

const EE = new EventEmitter();

common.expectsError(
  () => EE.emit('error', 'Accepts a string'),
  {
    code: 'ERR_UNHANDLED_ERROR',
    type: Error,
    message: "Unhandled error. ('Accepts a string')"
  }
);

common.expectsError(
  () => EE.emit('error', { message: 'Error!' }),
  {
    code: 'ERR_UNHANDLED_ERROR',
    type: Error,
    message: "Unhandled error. ({ message: 'Error!' })"
  }
);

common.expectsError(
  () => EE.emit('error', {
    message: 'Error!',
    [util.inspect.custom]() { throw null; }
  }),
  {
    code: 'ERR_UNHANDLED_ERROR',
    type: Error,
    message: "Unhandled error. ([object Object])"
  }
);
