// Copyright 2023 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_HEAP_PAGE_INL_H_
#define V8_HEAP_PAGE_INL_H_

#include "src/heap/page.h"
#include "src/heap/paged-spaces.h"
#include "src/heap/spaces.h"

namespace v8 {
namespace internal {

template <typename Callback>
void Page::ForAllFreeListCategories(Callback callback) {
  for (int i = kFirstCategory; i < owner()->free_list()->number_of_categories();
       i++) {
    callback(categories_[i]);
  }
}

void Page::MarkEvacuationCandidate() {
  DCHECK(!IsFlagSet(NEVER_EVACUATE));
  DCHECK_NULL(slot_set<OLD_TO_OLD>());
  DCHECK_NULL(typed_slot_set<OLD_TO_OLD>());
  SetFlag(EVACUATION_CANDIDATE);
  reinterpret_cast<PagedSpace*>(owner())->free_list()->EvictFreeListItems(this);
}

void Page::ClearEvacuationCandidate() {
  if (!IsFlagSet(COMPACTION_WAS_ABORTED)) {
    DCHECK_NULL(slot_set<OLD_TO_OLD>());
    DCHECK_NULL(typed_slot_set<OLD_TO_OLD>());
  }
  ClearFlag(EVACUATION_CANDIDATE);
  InitializeFreeListCategories();
}

}  // namespace internal
}  // namespace v8

#endif  // V8_HEAP_PAGE_INL_H_
