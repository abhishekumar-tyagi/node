// Copyright 2023 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/sandbox/indirect-pointer-table.h"

#include "src/execution/isolate.h"
#include "src/logging/counters.h"
#include "src/sandbox/indirect-pointer-table-inl.h"

#ifdef V8_COMPRESS_POINTERS

namespace v8 {
namespace internal {

uint32_t IndirectPointerTable::Sweep(Space* space, Counters* counters) {
  uint32_t num_live_entries = GenericSweep(space);
  counters->indirect_pointers_count()->AddSample(num_live_entries);
  return num_live_entries;
}

}  // namespace internal
}  // namespace v8

#endif  // V8_COMPRESS_POINTERS
