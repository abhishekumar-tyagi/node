'use strict';

const { isWindows } = require('../common');
const assert = require('assert');
const url = require('url');

{
  const fileURL = url.pathToFileURL('test/').href;
  assert.ok(fileURL.startsWith('file:///'));
  assert.ok(fileURL.endsWith('/'));
}

{
  const fileURL = url.pathToFileURL('test\\').href;
  assert.ok(fileURL.startsWith('file:///'));
  if (isWindows)
    assert.ok(fileURL.endsWith('/'));
  else
    assert.ok(fileURL.endsWith('%5C'));
}

{
  const fileURL = url.pathToFileURL('test/%').href;
  assert.ok(fileURL.includes('%25'));
}

{
  if (isWindows) {
    // UNC path: \\server\share\resource

    // Missing server:
    assert.throws(() => url.pathToFileURL('\\\\\\no-server'), {
      code: 'ERR_INVALID_ARG_VALUE',
    });

    // Missing share or resource:
    assert.throws(() => url.pathToFileURL('\\\\host'), {
      code: 'ERR_INVALID_ARG_VALUE',
    });

    // Regression test for direct String.prototype.startsWith call
    assert.throws(() => url.pathToFileURL([
      '\\\\',
      { [Symbol.toPrimitive]: () => 'blep\\blop' },
    ]), {
      code: 'ERR_INVALID_ARG_TYPE',
    });
    assert.throws(() => url.pathToFileURL(['\\\\', 'blep\\blop']), {
      code: 'ERR_INVALID_ARG_TYPE',
    });
    assert.throws(() => url.pathToFileURL({
      [Symbol.toPrimitive]: () => '\\\\blep\\blop',
    }), {
      code: 'ERR_INVALID_ARG_TYPE',
    });

  } else {
    // UNC paths on posix are considered a single path that has backslashes:
    const fileURL = url.pathToFileURL('\\\\nas\\share\\path.txt').href;
    assert.match(fileURL, /file:\/\/.+%5C%5Cnas%5Cshare%5Cpath\.txt$/);
  }
}

const windowsTestCases = [
  // Lowercase ascii alpha
  { path: 'C:\\foo', expected: 'file:///C:/foo' },
  // Uppercase ascii alpha
  { path: 'C:\\FOO', expected: 'file:///C:/FOO' },
  // dir
  { path: 'C:\\dir\\foo', expected: 'file:///C:/dir/foo' },
  // trailing separator
  { path: 'C:\\dir\\', expected: 'file:///C:/dir/' },
  // dot
  { path: 'C:\\foo.mjs', expected: 'file:///C:/foo.mjs' },
  // space
  { path: 'C:\\foo bar', expected: 'file:///C:/foo%20bar' },
  // question mark
  { path: 'C:\\foo?bar', expected: 'file:///C:/foo%3Fbar' },
  // number sign
  { path: 'C:\\foo#bar', expected: 'file:///C:/foo%23bar' },
  // ampersand
  { path: 'C:\\foo&bar', expected: 'file:///C:/foo&bar' },
  // equals
  { path: 'C:\\foo=bar', expected: 'file:///C:/foo=bar' },
  // colon
  { path: 'C:\\foo:bar', expected: 'file:///C:/foo:bar' },
  // semicolon
  { path: 'C:\\foo;bar', expected: 'file:///C:/foo;bar' },
  // percent
  { path: 'C:\\foo%bar', expected: 'file:///C:/foo%25bar' },
  // backslash
  { path: 'C:\\foo\\bar', expected: 'file:///C:/foo/bar' },
  // backspace
  { path: 'C:\\foo\bbar', expected: 'file:///C:/foo%08bar' },
  // tab
  { path: 'C:\\foo\tbar', expected: 'file:///C:/foo%09bar' },
  // newline
  { path: 'C:\\foo\nbar', expected: 'file:///C:/foo%0Abar' },
  // carriage return
  { path: 'C:\\foo\rbar', expected: 'file:///C:/foo%0Dbar' },
  // latin1
  { path: 'C:\\fóóbàr', expected: 'file:///C:/f%C3%B3%C3%B3b%C3%A0r' },
  // Euro sign (BMP code point)
  { path: 'C:\\€', expected: 'file:///C:/%E2%82%AC' },
  // Rocket emoji (non-BMP code point)
  { path: 'C:\\🚀', expected: 'file:///C:/%F0%9F%9A%80' },
  // UNC path (see https://docs.microsoft.com/en-us/archive/blogs/ie/file-uris-in-windows)
  { path: '\\\\nas\\My Docs\\File.doc', expected: 'file://nas/My%20Docs/File.doc' },
];
const posixTestCases = [
  // Lowercase ascii alpha
  { path: '/foo', expected: 'file:///foo' },
  // Uppercase ascii alpha
  { path: '/FOO', expected: 'file:///FOO' },
  // dir
  { path: '/dir/foo', expected: 'file:///dir/foo' },
  // trailing separator
  { path: '/dir/', expected: 'file:///dir/' },
  // dot
  { path: '/foo.mjs', expected: 'file:///foo.mjs' },
  // space
  { path: '/foo bar', expected: 'file:///foo%20bar' },
  // question mark
  { path: '/foo?bar', expected: 'file:///foo%3Fbar' },
  // number sign
  { path: '/foo#bar', expected: 'file:///foo%23bar' },
  // ampersand
  { path: '/foo&bar', expected: 'file:///foo&bar' },
  // equals
  { path: '/foo=bar', expected: 'file:///foo=bar' },
  // colon
  { path: '/foo:bar', expected: 'file:///foo:bar' },
  // semicolon
  { path: '/foo;bar', expected: 'file:///foo;bar' },
  // percent
  { path: '/foo%bar', expected: 'file:///foo%25bar' },
  // backslash
  { path: '/foo\\bar', expected: 'file:///foo%5Cbar' },
  // backspace
  { path: '/foo\bbar', expected: 'file:///foo%08bar' },
  // tab
  { path: '/foo\tbar', expected: 'file:///foo%09bar' },
  // newline
  { path: '/foo\nbar', expected: 'file:///foo%0Abar' },
  // carriage return
  { path: '/foo\rbar', expected: 'file:///foo%0Dbar' },
  // latin1
  { path: '/fóóbàr', expected: 'file:///f%C3%B3%C3%B3b%C3%A0r' },
  // Euro sign (BMP code point)
  { path: '/€', expected: 'file:///%E2%82%AC' },
  // Rocket emoji (non-BMP code point)
  { path: '/🚀', expected: 'file:///%F0%9F%9A%80' },
];

for (const { path, expected } of windowsTestCases) {
  const actual = url.pathToFileURL(path, { windows: true }).href;
  assert.strictEqual(actual, expected);
}

for (const { path, expected } of posixTestCases) {
  const actual = url.pathToFileURL(path, { windows: false }).href;
  assert.strictEqual(actual, expected);
}

const testCases = isWindows ? windowsTestCases : posixTestCases;

// Test when `options` is null
const whenNullActual = url.pathToFileURL(testCases[0].path, null);
assert.strictEqual(whenNullActual.href, testCases[0].expected);

for (const { path, expected } of testCases) {
  const actual = url.pathToFileURL(path).href;
  assert.strictEqual(actual, expected);
}

// Test for non-string parameter
{
  for (const badPath of [
    undefined, null, true, 42, 42n, Symbol('42'), NaN, {}, [], () => {},
    Promise.resolve('foo'),
    new Date(),
    new String('notPrimitive'),
    { toString() { return 'amObject'; } },
    { [Symbol.toPrimitive]: (hint) => 'amObject' },
  ]) {
    assert.throws(() => url.pathToFileURL(badPath), {
      code: 'ERR_INVALID_ARG_TYPE',
    });
  }
}
