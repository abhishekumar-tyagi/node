'use strict';

module.exports = {
  'case-sensitivity.any.js': {
    fail: {
      note: 'self.location is not available',
      expected: [
        'getEntriesByName values are case sensitive',
      ],
    },
  },
  'webtiming-resolution.any.js': {
    skip: 'flaky',
  },
};
