'use strict';
const common = require('../../common');
const assert = require('assert');
const test = require(`./build/${common.buildType}/test_conversions`);

const boolExpected = /boolean was expected/;
const numberExpected = /number was expected/;
const stringExpected = /string was expected/;

const testSym = Symbol('test');

assert.strictEqual(false, test.asBool(false));
assert.strictEqual(true, test.asBool(true));
assert.throws(() => test.asBool(undefined), boolExpected);
assert.throws(() => test.asBool(null), boolExpected);
assert.throws(() => test.asBool(Number.NaN), boolExpected);
assert.throws(() => test.asBool(0), boolExpected);
assert.throws(() => test.asBool(''), boolExpected);
assert.throws(() => test.asBool('0'), boolExpected);
assert.throws(() => test.asBool(1), boolExpected);
assert.throws(() => test.asBool('1'), boolExpected);
assert.throws(() => test.asBool('true'), boolExpected);
assert.throws(() => test.asBool({}), boolExpected);
assert.throws(() => test.asBool([]), boolExpected);
assert.throws(() => test.asBool(testSym), boolExpected);

[test.asInt32, test.asUInt32, test.asInt64].forEach((asInt) => {
  assert.strictEqual(0, asInt(0));
  assert.strictEqual(1, asInt(1));
  assert.strictEqual(1, asInt(1.0));
  assert.strictEqual(1, asInt(1.1));
  assert.strictEqual(1, asInt(1.9));
  assert.strictEqual(0, asInt(0.9));
  assert.strictEqual(999, asInt(999.9));
  assert.strictEqual(0, asInt(Number.NaN));
  assert.throws(() => asInt(undefined), numberExpected);
  assert.throws(() => asInt(null), numberExpected);
  assert.throws(() => asInt(false), numberExpected);
  assert.throws(() => asInt(''), numberExpected);
  assert.throws(() => asInt('1'), numberExpected);
  assert.throws(() => asInt({}), numberExpected);
  assert.throws(() => asInt([]), numberExpected);
  assert.throws(() => asInt(testSym), numberExpected);
});

assert.strictEqual(-1, test.asInt32(-1));
assert.strictEqual(-1, test.asInt64(-1));
assert.strictEqual(Math.pow(2, 32) - 1, test.asUInt32(-1));

assert.strictEqual(0, test.asDouble(0));
assert.strictEqual(1, test.asDouble(1));
assert.strictEqual(1.0, test.asDouble(1.0));
assert.strictEqual(1.1, test.asDouble(1.1));
assert.strictEqual(1.9, test.asDouble(1.9));
assert.strictEqual(0.9, test.asDouble(0.9));
assert.strictEqual(999.9, test.asDouble(999.9));
assert.strictEqual(-1, test.asDouble(-1));
assert.ok(Number.isNaN(test.asDouble(Number.NaN)));
assert.throws(() => test.asDouble(undefined), numberExpected);
assert.throws(() => test.asDouble(null), numberExpected);
assert.throws(() => test.asDouble(false), numberExpected);
assert.throws(() => test.asDouble(''), numberExpected);
assert.throws(() => test.asDouble('1'), numberExpected);
assert.throws(() => test.asDouble({}), numberExpected);
assert.throws(() => test.asDouble([]), numberExpected);
assert.throws(() => test.asDouble(testSym), numberExpected);

assert.strictEqual('', test.asString(''));
assert.strictEqual('test', test.asString('test'));
assert.throws(() => test.asString(undefined), stringExpected);
assert.throws(() => test.asString(null), stringExpected);
assert.throws(() => test.asString(false), stringExpected);
assert.throws(() => test.asString(1), stringExpected);
assert.throws(() => test.asString(1.1), stringExpected);
assert.throws(() => test.asString(Number.NaN), stringExpected);
assert.throws(() => test.asString({}), stringExpected);
assert.throws(() => test.asString([]), stringExpected);
assert.throws(() => test.asString(testSym), stringExpected);

assert.strictEqual(true, test.toBool(true));
assert.strictEqual(true, test.toBool(1));
assert.strictEqual(true, test.toBool(-1));
assert.strictEqual(true, test.toBool('true'));
assert.strictEqual(true, test.toBool('false'));
assert.strictEqual(true, test.toBool({}));
assert.strictEqual(true, test.toBool([]));
assert.strictEqual(true, test.toBool(testSym));
assert.strictEqual(false, test.toBool(false));
assert.strictEqual(false, test.toBool(undefined));
assert.strictEqual(false, test.toBool(null));
assert.strictEqual(false, test.toBool(0));
assert.strictEqual(false, test.toBool(Number.NaN));
assert.strictEqual(false, test.toBool(''));

assert.strictEqual(0, test.toNumber(0));
assert.strictEqual(1, test.toNumber(1));
assert.strictEqual(1.1, test.toNumber(1.1));
assert.strictEqual(-1, test.toNumber(-1));
assert.strictEqual(0, test.toNumber('0'));
assert.strictEqual(1, test.toNumber('1'));
assert.strictEqual(1.1, test.toNumber('1.1'));
assert.strictEqual(0, test.toNumber([]));
assert.strictEqual(0, test.toNumber(false));
assert.strictEqual(0, test.toNumber(null));
assert.strictEqual(0, test.toNumber(''));
assert.ok(Number.isNaN(test.toNumber(Number.NaN)));
assert.ok(Number.isNaN(test.toNumber({})));
assert.ok(Number.isNaN(test.toNumber(undefined)));
assert.throws(() => test.toNumber(testSym), TypeError);

assert.deepStrictEqual({}, test.toObject({}));
assert.deepStrictEqual({ 'test': 1 }, test.toObject({ 'test': 1 }));
assert.deepStrictEqual([], test.toObject([]));
assert.deepStrictEqual([ 1, 2, 3 ], test.toObject([ 1, 2, 3 ]));
assert.deepStrictEqual(new Boolean(false), test.toObject(false));
assert.deepStrictEqual(new Boolean(true), test.toObject(true));
assert.deepStrictEqual(new String(''), test.toObject(''));
assert.deepStrictEqual(new Number(0), test.toObject(0));
// TODO: Add test back in as soon as NaN is properly compared
// assert.deepStrictEqual(new Number(Number.NaN), test.toObject(Number.NaN));
assert.deepStrictEqual(new Object(testSym), test.toObject(testSym));
assert.notDeepStrictEqual(false, test.toObject(false));
assert.notDeepStrictEqual(true, test.toObject(true));
assert.notDeepStrictEqual('', test.toObject(''));
assert.notDeepStrictEqual(0, test.toObject(0));
assert.ok(!Number.isNaN(test.toObject(Number.NaN)));

assert.strictEqual('', test.toString(''));
assert.strictEqual('test', test.toString('test'));
assert.strictEqual('undefined', test.toString(undefined));
assert.strictEqual('null', test.toString(null));
assert.strictEqual('false', test.toString(false));
assert.strictEqual('true', test.toString(true));
assert.strictEqual('0', test.toString(0));
assert.strictEqual('1.1', test.toString(1.1));
assert.strictEqual('NaN', test.toString(Number.NaN));
assert.strictEqual('[object Object]', test.toString({}));
assert.strictEqual('test', test.toString({ toString: () => 'test' }));
assert.strictEqual('', test.toString([]));
assert.strictEqual('1,2,3', test.toString([ 1, 2, 3 ]));
assert.throws(() => test.toString(testSym), TypeError);
