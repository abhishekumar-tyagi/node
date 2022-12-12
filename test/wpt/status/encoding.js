'use strict';

module.exports = {
  'api-basics.any.js': {
    requires: [
      'small-icu',
    ],
  },
  'textdecoder-fatal-streaming.any.js': {
    requires: [
      'small-icu',
    ],
  },
  'textdecoder-fatal.any.js': {
    requires: [
      'small-icu',
    ],
  },
  'textdecoder-ignorebom.any.js': {
    requires: [
      'small-icu',
    ],
  },
  'textdecoder-streaming.any.js': {
    requires: [
      'small-icu',
    ],
  },
  'textdecoder-utf16-surrogates.any.js': {
    requires: [
      'small-icu',
    ],
  },
  'iso-2022-jp-decoder.any.js': {
    requires: [
      'full-icu',
    ],
    skip: 'iso-2022-jp decoder state handling bug: https://encoding.spec.whatwg.org/#iso-2022-jp-decoder',
  },
  'textdecoder-byte-order-marks.any.js': {
    requires: [
      'small-icu',
    ],
  },
  'textdecoder-fatal-single-byte.any.js': {
    requires: [
      'full-icu',
    ],
    skip: 'The iso-8859-16 encoding is not supported',
  },
  'textdecoder-labels.any.js': {
    requires: [
      'full-icu',
    ],
    skip: 'The iso-8859-16 encoding is not supported',
  },
  'textencoder-constructor-non-utf.any.js': {
    requires: [
      'full-icu',
    ],
    skip: 'The iso-8859-16 encoding is not supported',
  },
  'idlharness.any.js': {
    skip: 'No implementation of TextDecoderStream and TextEncoderStream',
  },
  'replacement-encodings.any.js': {
    skip: 'decoding-helpers.js needs XMLHttpRequest',
  },
  'unsupported-encodings.any.js': {
    skip: 'decoding-helpers.js needs XMLHttpRequest',
  },
  'streams/decode-ignore-bom.any.js': {
    requires: [
      'small-icu',
    ],
  },
  'streams/realms.window.js': {
    skip: 'window is not defined',
  },
  'streams/decode-attributes.any.js': {
    requires: [
      'full-icu',
    ],
  },
  'streams/decode-incomplete-input.any.js': {
    requires: [
      'small-icu',
    ],
  },
  'streams/decode-utf8.any.js': {
    requires: [
      'small-icu',
    ],
  },
  'streams/decode-bad-chunks.any.js': {
    fail: {
      expected: [
        'chunk of type undefined should error the stream',
      ],
    },
  },
  'streams/decode-non-utf8.any.js': {
    requires: [
      'full-icu',
    ],
  },
  'encodeInto.any.js': {
    requires: [
      'small-icu',
    ],
  },
  'textdecoder-copy.any.js': {
    requires: [
      'small-icu',
    ],
  },
  'legacy-mb-schinese/gbk/gbk-decoder.any.js': {
    requires: [
      'full-icu',
    ],
    skip: 'The gbk encoding is not supported',
  },
  'legacy-mb-schinese/gb18030/gb18030-decoder.any.js': {
    requires: [
      'full-icu',
    ],
    skip: 'The gb18030 encoding is not supported',
  },
  'textdecoder-arguments.any.js': {
    requires: [
      'small-icu',
    ],
  },
  'single-byte-decoder.window.js': {
    skip: 'document is not defined',
  },
  'textdecoder-eof.any.js': {
    requires: [
      'small-icu',
    ],
  },
  'unsupported-labels.window.js': {
    skip: 'document is not defined',
  },
};
