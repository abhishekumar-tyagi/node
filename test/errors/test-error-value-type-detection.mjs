// Flags: --expose-internals

import '../common/index.mjs';
import { strictEqual } from 'node:assert';
import errorsModule from 'internal/errors';


const { determineSpecificType } = errorsModule;

strictEqual(
  determineSpecificType(1n),
  'type bigint (1n)',
);

strictEqual(
  determineSpecificType(false),
  'type boolean (false)',
);

strictEqual(
  determineSpecificType(2),
  'type number (2)',
);

strictEqual(
  determineSpecificType(NaN),
  'type number (NaN)',
);

strictEqual(
  determineSpecificType(Infinity),
  'type number (Infinity)',
);

strictEqual(
  determineSpecificType(''),
  "type string ('')",
);

strictEqual(
  determineSpecificType(Symbol('foo')),
  'type symbol (Symbol(foo))',
);

strictEqual(
  determineSpecificType(function foo() {}),
  'function foo',
);

strictEqual(
  determineSpecificType(null),
  'null',
);

strictEqual(
  determineSpecificType(undefined),
  'undefined',
);

strictEqual(
  determineSpecificType([]),
  'an instance of Array',
);

strictEqual(
  determineSpecificType(new Array(0)),
  'an instance of Array',
);
strictEqual(
  determineSpecificType(new BigInt64Array(0)),
  'an instance of BigInt64Array',
);
strictEqual(
  determineSpecificType(new BigUint64Array(0)),
  'an instance of BigUint64Array',
);
strictEqual(
  determineSpecificType(new Int8Array(0)),
  'an instance of Int8Array',
);
strictEqual(
  determineSpecificType(new Int16Array(0)),
  'an instance of Int16Array',
);
strictEqual(
  determineSpecificType(new Int32Array(0)),
  'an instance of Int32Array',
);
strictEqual(
  determineSpecificType(new Float32Array(0)),
  'an instance of Float32Array',
);
strictEqual(
  determineSpecificType(new Float64Array(0)),
  'an instance of Float64Array',
);
strictEqual(
  determineSpecificType(new Uint8Array(0)),
  'an instance of Uint8Array',
);
strictEqual(
  determineSpecificType(new Uint8ClampedArray(0)),
  'an instance of Uint8ClampedArray',
);
strictEqual(
  determineSpecificType(new Uint16Array(0)),
  'an instance of Uint16Array',
);
strictEqual(
  determineSpecificType(new Uint32Array(0)),
  'an instance of Uint32Array',
);

strictEqual(
  determineSpecificType(new Date()),
  'an instance of Date',
);

strictEqual(
  determineSpecificType(new Map()),
  'an instance of Map',
);
strictEqual(
  determineSpecificType(new WeakMap()),
  'an instance of WeakMap',
);

strictEqual(
  determineSpecificType(Object.create(null)),
  'an instance of Object',
);
strictEqual(
  determineSpecificType({}),
  'an instance of Object',
);

strictEqual(
  determineSpecificType(Promise.resolve('foo')),
  'an instance of Promise',
);

strictEqual(
  determineSpecificType(new Set()),
  'an instance of Set',
);
strictEqual(
  determineSpecificType(new WeakSet()),
  'an instance of WeakSet',
);
