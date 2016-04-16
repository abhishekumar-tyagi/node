// Copyright 2014 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_COMPILER_COMMON_NODE_CACHE_H_
#define V8_COMPILER_COMMON_NODE_CACHE_H_

#include "src/compiler/node-cache.h"

namespace v8 {
namespace internal {

// Forward declarations.
class ExternalReference;
class HeapObject;
template <typename>
class Handle;


namespace compiler {

// Bundles various caches for common nodes.
class CommonNodeCache final {
 public:
  explicit CommonNodeCache(Zone* zone) : zone_(zone) {}
  ~CommonNodeCache() {}

  Node** FindInt32Constant(int32_t value) {
    return int32_constants_.Find(zone(), value);
  }

  Node** FindInt64Constant(int64_t value) {
    return int64_constants_.Find(zone(), value);
  }

  Node** FindFloat32Constant(float value) {
    // We canonicalize float constants at the bit representation level.
    return float32_constants_.Find(zone(), bit_cast<int32_t>(value));
  }

  Node** FindFloat64Constant(double value) {
    // We canonicalize double constants at the bit representation level.
    return float64_constants_.Find(zone(), bit_cast<int64_t>(value));
  }

  Node** FindExternalConstant(ExternalReference value);

  Node** FindNumberConstant(double value) {
    // We canonicalize double constants at the bit representation level.
    return number_constants_.Find(zone(), bit_cast<int64_t>(value));
  }

  Node** FindHeapConstant(Handle<HeapObject> value);

  // Return all nodes from the cache.
  void GetCachedNodes(ZoneVector<Node*>* nodes);

  Zone* zone() const { return zone_; }

 private:
  Int32NodeCache int32_constants_;
  Int64NodeCache int64_constants_;
  Int32NodeCache float32_constants_;
  Int64NodeCache float64_constants_;
  IntPtrNodeCache external_constants_;
  Int64NodeCache number_constants_;
  IntPtrNodeCache heap_constants_;
  Zone* const zone_;

  DISALLOW_COPY_AND_ASSIGN(CommonNodeCache);
};

}  // namespace compiler
}  // namespace internal
}  // namespace v8

#endif  // V8_COMPILER_COMMON_NODE_CACHE_H_
