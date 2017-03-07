'use strict';

// Flags: --expose-internals

require('../common');
const assert = require('assert');
const L = require('_linklist'); // eslint-disable-line no-restricted-modules
const internalL = require('internal/linkedlist');

assert.strictEqual(L, internalL);

const list = { name: 'list' };
const A = { name: 'A' };
const B = { name: 'B' };
const C = { name: 'C' };
const D = { name: 'D' };


L.init(list);
L.init(A);
L.init(B);
L.init(C);
L.init(D);

assert.ok(L.isEmpty(list));
assert.strictEqual(null, L.peek(list));

L.append(list, A);
// list -> A
assert.strictEqual(A, L.peek(list));

L.append(list, B);
// list -> A -> B
assert.strictEqual(A, L.peek(list));

L.append(list, C);
// list -> A -> B -> C
assert.strictEqual(A, L.peek(list));

L.append(list, D);
// list -> A -> B -> C -> D
assert.strictEqual(A, L.peek(list));

L.remove(A);
L.remove(B);
// B is already removed, so removing it again shouldn't hurt.
L.remove(B);
// list -> C -> D
assert.strictEqual(C, L.peek(list));

// Put B back on the list
L.append(list, B);
// list -> C -> D -> B
assert.strictEqual(C, L.peek(list));

L.remove(C);
// list -> D -> B
assert.strictEqual(D, L.peek(list));

L.remove(B);
// list -> D
assert.strictEqual(D, L.peek(list));

L.remove(D);
// list
assert.strictEqual(null, L.peek(list));


assert.ok(L.isEmpty(list));


L.append(list, D);
// list -> D
assert.strictEqual(D, L.peek(list));

L.append(list, C);
L.append(list, B);
L.append(list, A);
// list -> D -> C -> B -> A

// Append should REMOVE C from the list and append it to the end.
L.append(list, C);
// list -> D -> B -> A -> C

assert.strictEqual(D, L.peek(list));
assert.strictEqual(B, L.peek(D));
assert.strictEqual(A, L.peek(B));
assert.strictEqual(C, L.peek(A));
assert.strictEqual(list, L.peek(C));
