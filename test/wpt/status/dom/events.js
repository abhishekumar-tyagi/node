'use strict';

module.exports = {
  'AddEventListenerOptions-passive.any.js': {
    fail: {
      expected: [
        'preventDefault should be ignored if-and-only-if the passive option is true',
        'returnValue should be ignored if-and-only-if the passive option is true',
        'passive behavior of one listener should be unaffected by the presence of other listeners',
      ],
    },
  },
  'Event-dispatch-listener-order.window.js': {
    skip: 'document is not defined',
  },
  'EventListener-addEventListener.sub.window.js': {
    skip: 'document is not defined',
  },
  'EventTarget-removeEventListener.any.js': {
    skip: 'globalThis.removeEventListener is not a function',
  },
  'event-global-extra.window.js': {
    skip: 'document is not defined',
  },
  'event-global-set-before-handleEvent-lookup.window.js': {
    skip: 'window is not defined',
  },
  'event-global.worker.js': {
    skip: 'importScripts is not defined',
  },
  'legacy-pre-activation-behavior.window.js': {
    skip: 'document is not defined',
  },
  'relatedTarget.window.js': {
    skip: 'document is not defined',
  },
};
