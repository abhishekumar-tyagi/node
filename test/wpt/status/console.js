'use strict';

module.exports = {
  'idlharness.any.js': {
    fail: {
      flaky: [
        'console namespace: operation assert(optional boolean, any...)',
        'console namespace: operation table(optional any, optional sequence<DOMString>)',
        'console namespace: operation dir(optional any, optional object?)',
      ],
    },
  },
};
