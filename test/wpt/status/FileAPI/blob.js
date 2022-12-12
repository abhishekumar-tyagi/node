'use strict';

module.exports = {
  'Blob-constructor-dom.window.js': {
    skip: 'Depends on DOM API',
  },
  'Blob-constructor.any.js': {
    fail: {
      note: 'Depends on File API',
      expected: [
        'A plain object with @@iterator should be treated as a sequence for the blobParts argument.',
        'A plain object with @@iterator and a length property should be treated as a sequence for the blobParts argument.',
        'A String object should be treated as a sequence for the blobParts argument.',
        'A Uint8Array object should be treated as a sequence for the blobParts argument.',
        'Getters and value conversions should happen in order until an exception is thrown.',
        'Changes to the blobParts array should be reflected in the returned Blob (pop).',
        'Changes to the blobParts array should be reflected in the returned Blob (unshift).',
        'ToString should be called on elements of the blobParts array.',
        'ArrayBuffer elements of the blobParts array should be supported.',
        'Passing typed arrays as elements of the blobParts array should work.',
        'Passing a Float64Array as element of the blobParts array should work.',
        'Array with two blobs',
        'Array with two buffers',
        'Array with two bufferviews',
        'Array with mixed types',
        'options properties should be accessed in lexicographic order.',
        'Arguments should be evaluated from left to right.',
        'Passing null (index 0) for options should use the defaults.',
        'Passing null (index 0) for options should use the defaults (with newlines).',
        'Passing undefined (index 1) for options should use the defaults.',
        'Passing undefined (index 1) for options should use the defaults (with newlines).',
        'Passing object "[object Object]" (index 2) for options should use the defaults.',
        'Passing object "[object Object]" (index 2) for options should use the defaults (with newlines).',
        'Passing object "[object Object]" (index 3) for options should use the defaults.',
        'Passing object "[object Object]" (index 3) for options should use the defaults (with newlines).',
        'Passing object "/regex/" (index 4) for options should use the defaults.',
        'Passing object "/regex/" (index 4) for options should use the defaults (with newlines).',
        'Passing function "function() {}" (index 5) for options should use the defaults.',
        'Passing function "function() {}" (index 5) for options should use the defaults (with newlines).',
      ],
    },
  },
  'Blob-in-worker.worker.js': {
    skip: 'Depends on Web Workers API',
  },
  'Blob-slice.any.js': {
    skip: 'Depends on File API',
  },
};
