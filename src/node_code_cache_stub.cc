
#include "node_code_cache.h"

// This is supposed to be generated by tools/generate_code_cache.js
// The stub here is used when configure is run without `--code-cache-path`

namespace node {
void DefineCodeCache(Environment* env, v8::Local<v8::Object> target) {
  // When we do not produce code cache for builtin modules,
  // `internalBinding('code_cache')` returns an empty object
  // (here as `target`) so this is a noop.
}

void DefineCodeCacheHash(Environment* env, v8::Local<v8::Object> target) {
  // When we do not produce code cache for builtin modules,
  // `internalBinding('code_cache_hash')` returns an empty object
  // (here as `target`) so this is a noop.
}

}  // namespace node
