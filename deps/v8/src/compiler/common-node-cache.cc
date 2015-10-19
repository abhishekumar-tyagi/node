// Copyright 2014 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/compiler/common-node-cache.h"

#include "src/assembler.h"
#include "src/compiler/node.h"
#include "src/objects-inl.h"  // TODO(mstarzinger): Temporary cycle breaker!

namespace v8 {
namespace internal {
namespace compiler {

Node** CommonNodeCache::FindExternalConstant(ExternalReference value) {
  return external_constants_.Find(zone(), bit_cast<intptr_t>(value.address()));
}


void CommonNodeCache::GetCachedNodes(ZoneVector<Node*>* nodes) {
  int32_constants_.GetCachedNodes(nodes);
  int64_constants_.GetCachedNodes(nodes);
  float32_constants_.GetCachedNodes(nodes);
  float64_constants_.GetCachedNodes(nodes);
  external_constants_.GetCachedNodes(nodes);
  number_constants_.GetCachedNodes(nodes);
}

}  // namespace compiler
}  // namespace internal
}  // namespace v8
