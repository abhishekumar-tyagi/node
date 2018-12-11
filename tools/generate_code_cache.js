'use strict';

// Flags: --expose-internals

// This file generates the code cache for builtin modules and
// writes them into static char arrays of a C++ file that can be
// compiled into the binary using the `--code-cache-path` option
// of `configure`.

const {
  getCodeCache,
  compileFunction,
  cachableBuiltins
} = require('internal/bootstrap/cache');

const {
  types: {
    isUint8Array
  }
} = require('util');

const fs = require('fs');

const resultPath = process.argv[2];
if (!resultPath) {
  console.error(`Usage: ${process.argv[0]} ${process.argv[1]}` +
                'path/to/node_code_cache.cc');
  process.exit(1);
}

/**
 * Format a number of a size in bytes into human-readable strings
 * @param {number} num
 * @return {string}
 */
function formatSize(num) {
  if (num < 1024) {
    return `${(num).toFixed(2)}B`;
  } else if (num < 1024 ** 2) {
    return `${(num / 1024).toFixed(2)}KB`;
  } else if (num < 1024 ** 3) {
    return `${(num / (1024 ** 2)).toFixed(2)}MB`;
  } else {
    return `${(num / (1024 ** 3)).toFixed(2)}GB`;
  }
}

/**
 * Generates the source code of definitions of the char arrays
 * that contains the code cache and the source code of the
 * initializers of the code cache.
 *
 * @param {string} key ID of the builtin module
 * @param {Uint8Array} cache Code cache of the builtin module
 * @return { definition: string, initializer: string }
 */
function getInitalizer(key, cache) {
  const defName = `${key.replace(/\//g, '_').replace(/-/g, '_')}_raw`;
  const definition = `static const uint8_t ${defName}[] = {\n` +
                     `${cache.join(',')}\n};`;
  const dataDef = 'std::make_unique<v8::ScriptCompiler::CachedData>(' +
                  `${defName}, static_cast<int>(arraysize(${defName})), ` +
                  'policy)';
  const initializer =
    'code_cache_.emplace(\n' +
    `  "${key}",\n` +
    `  ${dataDef}\n` +
    ');';
  return {
    definition, initializer
  };
}

const cacheDefinitions = [];
const cacheInitializers = [];
let totalCacheSize = 0;

function lexical(a, b) {
  if (a < b) {
    return -1;
  }
  if (a > b) {
    return 1;
  }
  return 0;
}

for (const key of cachableBuiltins.sort(lexical)) {
  compileFunction(key);  // compile it
  const cachedData = getCodeCache(key);
  if (!isUint8Array(cachedData)) {
    console.error(`Failed to generate code cache for '${key}'`);
    process.exit(1);
  }

  const size = cachedData.byteLength;
  totalCacheSize += size;
  const {
    definition, initializer,
  } = getInitalizer(key, cachedData);
  cacheDefinitions.push(definition);
  cacheInitializers.push(initializer);
  console.log(`Generated cache for '${key}', size = ${formatSize(size)}` +
              `, total = ${formatSize(totalCacheSize)}`);
}

const result = `#include "node_native_module.h"
#include "node_internals.h"

// This file is generated by tools/generate_code_cache.js
// and is used when configure is run with \`--code-cache-path\`

namespace node {
namespace native_module {
${cacheDefinitions.join('\n\n')}

void NativeModuleLoader::LoadCodeCache() {
  auto policy = v8::ScriptCompiler::CachedData::BufferPolicy::BufferNotOwned;
  ${cacheInitializers.join('\n  ')}
}

}  // namespace native_module
}  // namespace node
`;

fs.writeFileSync(resultPath, result);
console.log(`Generated code cache C++ file to ${resultPath}`);
