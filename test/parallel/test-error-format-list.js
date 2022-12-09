// Flags: --expose-internals
'use strict';

const common = require('../common');
const { strictEqual } = require('node:assert');
const { formatList } = require('internal/errors');

if (common.hasIntl) {
  const and = new Intl.ListFormat('en', { style: 'long', type: 'conjunction' });
  const or = new Intl.ListFormat('en', { style: 'long', type: 'disjunction' });

  const input = ['apple', 'banana', 'orange'];
  for (let i = 0; i < input.length; i++) {
    const slicedInput = input.slice(0, i);
    strictEqual(formatList(slicedInput), and.format(slicedInput));
    strictEqual(formatList(slicedInput, 'or'), or.format(slicedInput));
  }
}
