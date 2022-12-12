'use strict';

module.exports = {
  'queue-microtask-exceptions.any.js': {
    skip: 'Node.js does not have a global addEventListener function',
  },
  'queue-microtask.window.js': {
    skip: 'MutationObserver is not implemented',
  },
};
