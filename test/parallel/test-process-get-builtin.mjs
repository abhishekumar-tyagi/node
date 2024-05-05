import '../common/index.mjs';
import assert from 'node:assert';
import { builtinModules } from 'node:module';

const { getBuiltin } = globalThis.process;
for (const invalid of [1, undefined, null, false, [], {}, () => {}, Symbol('test')]) {
  assert.throws(() => getBuiltin(invalid), { code: 'ERR_INVALID_ARG_TYPE' });
}

for (const invalid of [
  'invalid', 'node:test', 'node:fs', 'internal/bootstrap/realm',
  'internal/deps/undici/undici', 'internal/util',
]) {
  assert.throws(() => getBuiltin(invalid), { code: 'ERR_UNKNOWN_BUILTIN_MODULE' });
}

// Check that createRequire()(id) returns the same thing as getBuiltin(id).
const require = getBuiltin('module').createRequire(import.meta.url);
for (const id of builtinModules) {
  assert.strictEqual(getBuiltin(id), require(id));
}
// builtinModules does not include 'test' which requires the node: prefix.
const ids = builtinModules.concat(['test']);
for (const id of builtinModules) {
  assert.strictEqual(getBuiltin(id), require(`node:${id}`));
}

// Check that import(id).default returns the same thing as getBuiltin(id).
for (const id of ids) {
  const imported = await import(`node:${id}`);
  assert.strictEqual(getBuiltin(id), imported.default);
}
