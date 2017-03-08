'use strict';

const common = require('../common');
const assert = require('assert');
const URLSearchParams = require('url').URLSearchParams;
const {
  test, assert_equals, assert_true,
  assert_false, assert_throws, assert_array_equals
} = common.WPT;

/* eslint-disable */
var params;  // Strict mode fix for WPT.
/* WPT Refs:
   https://github.com/w3c/web-platform-tests/blob/405394a/url/urlsearchparams-constructor.html
   License: http://www.w3.org/Consortium/Legal/2008/04-testsuite-copyright.html
*/
test(function() {
    var params = new URLSearchParams();
    assert_equals(params + '', '');
    params = new URLSearchParams('');
    assert_equals(params + '', '');
    params = new URLSearchParams('a=b');
    assert_equals(params + '', 'a=b');
    params = new URLSearchParams(params);
    assert_equals(params + '', 'a=b');
}, 'Basic URLSearchParams construction');

test(function() {
    var params = new URLSearchParams()
    assert_equals(params.toString(), "")
}, "URLSearchParams constructor, no arguments")

// test(() => {
//     params = new URLSearchParams(DOMException.prototype);
//     assert_equals(params.toString(), "INDEX_SIZE_ERR=1&DOMSTRING_SIZE_ERR=2&HIERARCHY_REQUEST_ERR=3&WRONG_DOCUMENT_ERR=4&INVALID_CHARACTER_ERR=5&NO_DATA_ALLOWED_ERR=6&NO_MODIFICATION_ALLOWED_ERR=7&NOT_FOUND_ERR=8&NOT_SUPPORTED_ERR=9&INUSE_ATTRIBUTE_ERR=10&INVALID_STATE_ERR=11&SYNTAX_ERR=12&INVALID_MODIFICATION_ERR=13&NAMESPACE_ERR=14&INVALID_ACCESS_ERR=15&VALIDATION_ERR=16&TYPE_MISMATCH_ERR=17&SECURITY_ERR=18&NETWORK_ERR=19&ABORT_ERR=20&URL_MISMATCH_ERR=21&QUOTA_EXCEEDED_ERR=22&TIMEOUT_ERR=23&INVALID_NODE_TYPE_ERR=24&DATA_CLONE_ERR=25")
// }, "URLSearchParams constructor, DOMException.prototype as argument")

test(() => {
    params = new URLSearchParams('');
    assert_true(params != null, 'constructor returned non-null value.');
    assert_equals(params.__proto__, URLSearchParams.prototype, 'expected URLSearchParams.prototype as prototype.');
}, "URLSearchParams constructor, empty string as argument")

test(() => {
    params = new URLSearchParams({});
    assert_equals(params + '', "");
}, 'URLSearchParams constructor, {} as argument');

test(function() {
    var params = new URLSearchParams('a=b');
    assert_true(params != null, 'constructor returned non-null value.');
    assert_true(params.has('a'), 'Search params object has name "a"');
    assert_false(params.has('b'), 'Search params object has not got name "b"');
    var params = new URLSearchParams('a=b&c');
    assert_true(params != null, 'constructor returned non-null value.');
    assert_true(params.has('a'), 'Search params object has name "a"');
    assert_true(params.has('c'), 'Search params object has name "c"');
    var params = new URLSearchParams('&a&&& &&&&&a+b=& c&m%c3%b8%c3%b8');
    assert_true(params != null, 'constructor returned non-null value.');
    assert_true(params.has('a'), 'Search params object has name "a"');
    assert_true(params.has('a b'), 'Search params object has name "a b"');
    assert_true(params.has(' '), 'Search params object has name " "');
    assert_false(params.has('c'), 'Search params object did not have the name "c"');
    assert_true(params.has(' c'), 'Search params object has name " c"');
    assert_true(params.has('møø'), 'Search params object has name "møø"');
}, 'URLSearchParams constructor, string.');

test(function() {
    var seed = new URLSearchParams('a=b&c=d');
    var params = new URLSearchParams(seed);
    assert_true(params != null, 'constructor returned non-null value.');
    assert_equals(params.get('a'), 'b');
    assert_equals(params.get('c'), 'd');
    assert_false(params.has('d'));
    // The name-value pairs are copied when created; later updates
    // should not be observable.
    seed.append('e', 'f');
    assert_false(params.has('e'));
    params.append('g', 'h');
    assert_false(seed.has('g'));
}, 'URLSearchParams constructor, object.');

test(function() {
    var params = new URLSearchParams('a=b+c');
    assert_equals(params.get('a'), 'b c');
    params = new URLSearchParams('a+b=c');
    assert_equals(params.get('a b'), 'c');
}, 'Parse +');

test(function() {
    var params = new URLSearchParams('a=b c');
    assert_equals(params.get('a'), 'b c');
    params = new URLSearchParams('a b=c');
    assert_equals(params.get('a b'), 'c');
}, 'Parse space');

test(function() {
    var params = new URLSearchParams('a=b%20c');
    assert_equals(params.get('a'), 'b c');
    params = new URLSearchParams('a%20b=c');
    assert_equals(params.get('a b'), 'c');
}, 'Parse %20');

test(function() {
    var params = new URLSearchParams('a=b\0c');
    assert_equals(params.get('a'), 'b\0c');
    params = new URLSearchParams('a\0b=c');
    assert_equals(params.get('a\0b'), 'c');
}, 'Parse \\0');

test(function() {
    var params = new URLSearchParams('a=b%00c');
    assert_equals(params.get('a'), 'b\0c');
    params = new URLSearchParams('a%00b=c');
    assert_equals(params.get('a\0b'), 'c');
}, 'Parse %00');

test(function() {
    var params = new URLSearchParams('a=b\u2384');
    assert_equals(params.get('a'), 'b\u2384');
    params = new URLSearchParams('a\u2384b=c');
    assert_equals(params.get('a\u2384b'), 'c');
}, 'Parse \u2384');  // Unicode Character 'COMPOSITION SYMBOL' (U+2384)

test(function() {
    var params = new URLSearchParams('a=b%e2%8e%84');
    assert_equals(params.get('a'), 'b\u2384');
    params = new URLSearchParams('a%e2%8e%84b=c');
    assert_equals(params.get('a\u2384b'), 'c');
}, 'Parse %e2%8e%84');  // Unicode Character 'COMPOSITION SYMBOL' (U+2384)

test(function() {
    var params = new URLSearchParams('a=b\uD83D\uDCA9c');
    assert_equals(params.get('a'), 'b\uD83D\uDCA9c');
    params = new URLSearchParams('a\uD83D\uDCA9b=c');
    assert_equals(params.get('a\uD83D\uDCA9b'), 'c');
}, 'Parse \uD83D\uDCA9');  // Unicode Character 'PILE OF POO' (U+1F4A9)

test(function() {
    var params = new URLSearchParams('a=b%f0%9f%92%a9c');
    assert_equals(params.get('a'), 'b\uD83D\uDCA9c');
    params = new URLSearchParams('a%f0%9f%92%a9b=c');
    assert_equals(params.get('a\uD83D\uDCA9b'), 'c');
}, 'Parse %f0%9f%92%a9');  // Unicode Character 'PILE OF POO' (U+1F4A9)

test(function() {
    var params = new URLSearchParams([]);
    assert_true(params != null, 'constructor returned non-null value.');
    params = new URLSearchParams([['a', 'b'], ['c', 'd']]);
    assert_equals(params.get("a"), "b");
    assert_equals(params.get("c"), "d");
    assert_throws(new TypeError(), function() { new URLSearchParams([[1]]); });
    assert_throws(new TypeError(), function() { new URLSearchParams([[1,2,3]]); });
}, "Constructor with sequence of sequences of strings");

[
//   { "input": {"+": "%C2"}, "output": [[" ", "\uFFFD"]], "name": "object with +" },
  { "input": {c: "x", a: "?"}, "output": [["c", "x"], ["a", "?"]], "name": "object with two keys" },
  { "input": [["c", "x"], ["a", "?"]], "output": [["c", "x"], ["a", "?"]], "name": "array with two keys" }
].forEach((val) => {
    test(() => {
        let params = new URLSearchParams(val.input),
            i = 0
        for (let param of params) {
            assert_array_equals(param, val.output[i])
            i++
        }
    }, "Construct with " + val.name)
})

test(() => {
  params = new URLSearchParams()
  params[Symbol.iterator] = function *() {
    yield ["a", "b"]
  }
  let params2 = new URLSearchParams(params)
  assert_equals(params2.get("a"), "b")
}, "Custom [Symbol.iterator]")
/* eslint-enable */

// Tests below are not from WPT.
{
//   assert.throws(() => {
//     new URLSearchParams({
//         toString() { throw new TypeError('Illegal invocation'); }
//     });
//   }, TypeError);
}

{
  let params;
  // URLSearchParams constructor, undefined and null as argument
  params = new URLSearchParams(undefined);
  assert.strictEqual(params.toString(), '');
  params = new URLSearchParams(null);
  assert.strictEqual(params.toString(), '');
  assert.throws(() => new URLSearchParams([[1]]),
                /^TypeError: Each query pair must be a name\/value tuple$/);
  assert.throws(() => new URLSearchParams([[1, 2, 3]]),
                /^TypeError: Each query pair must be a name\/value tuple$/);
  assert.throws(() => new URLSearchParams({ [Symbol.iterator]: 42 }),
                /^TypeError: Query pairs must be iterable$/);
  assert.throws(() => new URLSearchParams([{}]),
                /^TypeError: Each query pair must be iterable$/);
  assert.throws(() => new URLSearchParams(['a']),
                /^TypeError: Each query pair must be iterable$/);
  assert.throws(() => new URLSearchParams([{ [Symbol.iterator]: 42 }]),
                /^TypeError: Each query pair must be iterable$/);
}

{
  const obj = {
    toString() { throw new Error('toString'); },
    valueOf() { throw new Error('valueOf'); }
  };
  const sym = Symbol();

  assert.throws(() => new URLSearchParams({ a: obj }), /^Error: toString$/);
  assert.throws(() => new URLSearchParams([['a', obj]]), /^Error: toString$/);
  assert.throws(() => new URLSearchParams(sym),
                /^TypeError: Cannot convert a Symbol value to a string$/);
  assert.throws(() => new URLSearchParams({ a: sym }),
                /^TypeError: Cannot convert a Symbol value to a string$/);
  assert.throws(() => new URLSearchParams([[sym, 'a']]),
                /^TypeError: Cannot convert a Symbol value to a string$/);
  assert.throws(() => new URLSearchParams([['a', sym]]),
                /^TypeError: Cannot convert a Symbol value to a string$/);
}
