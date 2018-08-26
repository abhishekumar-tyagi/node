// Flags: --experimental-vm-modules
'use strict';
require('../common');
const fixtures = require('../common/fixtures');
const assert = require('assert');
const { types, inspect } = require('util');
const vm = require('vm');
const { JSStream } = process.binding('js_stream');

const external = (new JSStream())._externalStream;
const wasmBuffer = fixtures.readSync('test.wasm');

for (const [ value, _method ] of [
  [ external, 'isExternal' ],
  [ new Date() ],
  [ (function() { return arguments; })(), 'isArgumentsObject' ],
  [ new Boolean(), 'isBooleanObject' ],
  [ new Number(), 'isNumberObject' ],
  [ new String(), 'isStringObject' ],
  [ Object(Symbol()), 'isSymbolObject' ],
  [ Object(BigInt(0)), 'isBigIntObject' ],
  [ new Error(), 'isNativeError' ],
  [ new RegExp() ],
  [ async function() {}, 'isAsyncFunction' ],
  [ function*() {}, 'isGeneratorFunction' ],
  [ (function*() {})(), 'isGeneratorObject' ],
  [ Promise.resolve() ],
  [ new Map() ],
  [ new Set() ],
  [ (new Map())[Symbol.iterator](), 'isMapIterator' ],
  [ (new Set())[Symbol.iterator](), 'isSetIterator' ],
  [ new WeakMap() ],
  [ new WeakSet() ],
  [ new ArrayBuffer() ],
  [ new Uint8Array() ],
  [ new Uint8ClampedArray() ],
  [ new Uint16Array() ],
  [ new Uint32Array() ],
  [ new Int8Array() ],
  [ new Int16Array() ],
  [ new Int32Array() ],
  [ new Float32Array() ],
  [ new Float64Array() ],
  [ new BigInt64Array() ],
  [ new BigUint64Array() ],
  [ Object.defineProperty(new Uint8Array(),
                          Symbol.toStringTag,
                          { value: 'foo' }) ],
  [ new DataView(new ArrayBuffer()) ],
  [ new SharedArrayBuffer() ],
  [ new Proxy({}, {}), 'isProxy' ],
  [ new WebAssembly.Module(wasmBuffer), 'isWebAssemblyCompiledModule' ],
]) {
  const method = _method || `is${value.constructor.name}`;
  assert(method in types, `Missing ${method} for ${inspect(value)}`);
  assert(types[method](value), `Want ${inspect(value)} to match ${method}`);

  for (const key of Object.keys(types)) {
    if ((types.isArrayBufferView(value) ||
         types.isAnyArrayBuffer(value)) && key.includes('Array') ||
         key === 'isBoxedPrimitive') {
      continue;
    }

    assert.strictEqual(types[key](value),
                       key === method,
                       `${inspect(value)}: ${key}, ` +
                       `${method}, ${types[key](value)}`);
  }
}

// Check boxed primitives.
[
  new Boolean(),
  new Number(),
  new String(),
  Object(Symbol()),
  Object(BigInt(0))
].forEach((entry) => assert(types.isBoxedPrimitive(entry)));

{
  assert(!types.isUint8Array({ [Symbol.toStringTag]: 'Uint8Array' }));
  assert(types.isUint8Array(vm.runInNewContext('new Uint8Array')));
}

{
  const primitive = true;
  const arrayBuffer = new ArrayBuffer();
  const dataView = new DataView(arrayBuffer);
  const int32Array = new Int32Array(arrayBuffer);
  const uint8Array = new Uint8Array(arrayBuffer);
  const buffer = Buffer.from(arrayBuffer);

  const fakeDataView = Object.create(DataView.prototype);
  const fakeInt32Array = Object.create(Int32Array.prototype);
  const fakeUint8Array = Object.create(Uint8Array.prototype);
  const fakeBuffer = Object.create(Buffer.prototype);

  const stealthyDataView =
      Object.setPrototypeOf(new DataView(arrayBuffer), Uint8Array.prototype);
  const stealthyInt32Array =
      Object.setPrototypeOf(new Int32Array(arrayBuffer), uint8Array);
  const stealthyUint8Array =
      Object.setPrototypeOf(new Uint8Array(arrayBuffer), ArrayBuffer.prototype);

  const all = [
    primitive, arrayBuffer, dataView, int32Array, uint8Array, buffer,
    fakeDataView, fakeInt32Array, fakeUint8Array, fakeBuffer,
    stealthyDataView, stealthyInt32Array, stealthyUint8Array
  ];

  const expected = {
    isArrayBufferView: [
      dataView, int32Array, uint8Array, buffer,
      stealthyDataView, stealthyInt32Array, stealthyUint8Array
    ],
    isTypedArray: [
      int32Array, uint8Array, buffer, stealthyInt32Array, stealthyUint8Array
    ],
    isUint8Array: [
      uint8Array, buffer, stealthyUint8Array
    ]
  };

  for (const testedFunc of Object.keys(expected)) {
    const func = types[testedFunc];
    const yup = [];
    for (const value of all) {
      if (func(value)) {
        yup.push(value);
      }
    }
    console.log('Testing', testedFunc);
    assert.deepStrictEqual(yup, expected[testedFunc]);
  }
}

(async () => {
  const m = new vm.SourceTextModule('');
  await m.link(() => 0);
  m.instantiate();
  await m.evaluate();
  assert.ok(types.isModuleNamespaceObject(m.namespace));
})();
