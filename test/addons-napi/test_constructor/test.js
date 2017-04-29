'use strict';
const common = require('../../common');
const assert = require('assert');

// Testing api calls for a constructor that defines properties
const TestConstructor = require(`./build/${common.buildType}/test_constructor`);
const test_object = new TestConstructor();

assert.strictEqual(test_object.echo('hello'), 'hello');

test_object.readwriteValue = 1;
assert.strictEqual(test_object.readwriteValue, 1);
test_object.readwriteValue = 2;
assert.strictEqual(test_object.readwriteValue, 2);

assert.throws(() => { test_object.readonlyValue = 3; }, TypeError);

assert.ok(test_object.hiddenValue);

// Properties with napi_enumerable attribute should be enumerable.
const propertyNames = [];
for (const name in test_object) {
  propertyNames.push(name);
}
assert.ok(propertyNames.includes('echo') >= 0);
assert.ok(propertyNames.includes('readwriteValue') >= 0);
assert.ok(propertyNames.includes('readonlyValue') >= 0);
assert.ok(propertyNames.includes('hiddenValue') < 0);
assert.ok(propertyNames.includes('readwriteAccessor1') < 0);
assert.ok(propertyNames.includes('readwriteAccessor2') < 0);
assert.ok(propertyNames.includes('readonlyAccessor1') < 0);
assert.ok(propertyNames.includes('readonlyAccessor2') < 0);

// The napi_writable attribute should be ignored for accessors.
test_object.readwriteAccessor1 = 1;
assert.strictEqual(test_object.readwriteAccessor1, 1);
assert.strictEqual(test_object.readonlyAccessor1, 1);
assert.throws(() => { test_object.readonlyAccessor1 = 3; }, TypeError);
test_object.readwriteAccessor2 = 2;
assert.strictEqual(test_object.readwriteAccessor2, 2);
assert.strictEqual(test_object.readonlyAccessor2, 2);
assert.throws(() => { test_object.readonlyAccessor2 = 3; }, TypeError);
