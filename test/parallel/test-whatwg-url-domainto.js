'use strict';
const common = require('../common');

if (!common.hasIntl) {
  common.skip('missing Intl');
  return;
}

const assert = require('assert');
const { domainToASCII, domainToUnicode } = require('url');

// Tests below are not from WPT.
const tests = require('../fixtures/url-idna.js');

{
  const expectedError = common.expectsError(
      { code: 'ERR_MISSING_ARGS', type: TypeError });
  assert.throws(() => domainToASCII(), expectedError);
  assert.throws(() => domainToUnicode(), expectedError);
  assert.strictEqual(domainToASCII(undefined), 'undefined');
  assert.strictEqual(domainToUnicode(undefined), 'undefined');
}

{
  for (const [i, { ascii, unicode }] of tests.valid.entries()) {
    assert.strictEqual(ascii, domainToASCII(unicode),
                       `domainToASCII(${i + 1})`);
    assert.strictEqual(unicode, domainToUnicode(ascii),
                       `domainToUnicode(${i + 1})`);
    assert.strictEqual(ascii, domainToASCII(domainToUnicode(ascii)),
                       `domainToASCII(domainToUnicode(${i + 1}))`);
    assert.strictEqual(unicode, domainToUnicode(domainToASCII(unicode)),
                       `domainToUnicode(domainToASCII(${i + 1}))`);
  }
}

{
  for (const [i, url] of tests.invalid.entries()) {
    assert.strictEqual(domainToASCII(url), '', `Invalid case ${i + 1}`);
    assert.strictEqual(domainToUnicode(url), '', `Invalid case ${i + 1}`);
  }
}
