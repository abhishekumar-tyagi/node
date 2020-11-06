// Copyright 2017 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/base/page-allocator.h"

#include "src/base/platform/platform.h"

#if V8_OS_MACOSX
#include <sys/mman.h>  // For MAP_JIT.
#endif

namespace v8 {
namespace base {

#define STATIC_ASSERT_ENUM(a, b)                            \
  static_assert(static_cast<int>(a) == static_cast<int>(b), \
                "mismatching enum: " #a)

STATIC_ASSERT_ENUM(PageAllocator::kNoAccess,
                   base::OS::MemoryPermission::kNoAccess);
STATIC_ASSERT_ENUM(PageAllocator::kReadWrite,
                   base::OS::MemoryPermission::kReadWrite);
STATIC_ASSERT_ENUM(PageAllocator::kReadWriteExecute,
                   base::OS::MemoryPermission::kReadWriteExecute);
STATIC_ASSERT_ENUM(PageAllocator::kReadExecute,
                   base::OS::MemoryPermission::kReadExecute);
STATIC_ASSERT_ENUM(PageAllocator::kNoAccessWillJitLater,
                   base::OS::MemoryPermission::kNoAccessWillJitLater);

#undef STATIC_ASSERT_ENUM

PageAllocator::PageAllocator()
    : allocate_page_size_(base::OS::AllocatePageSize()),
      commit_page_size_(base::OS::CommitPageSize()) {}

void PageAllocator::SetRandomMmapSeed(int64_t seed) {
  base::OS::SetRandomMmapSeed(seed);
}

void* PageAllocator::GetRandomMmapAddr() {
  return base::OS::GetRandomMmapAddr();
}

void* PageAllocator::AllocatePages(void* hint, size_t size, size_t alignment,
                                   PageAllocator::Permission access) {
#if !(V8_OS_MACOSX && V8_HOST_ARCH_ARM64 && defined(MAP_JIT))
  // kNoAccessWillJitLater is only used on Apple Silicon. Map it to regular
  // kNoAccess on other platforms, so code doesn't have to handle both enum
  // values.
  if (access == PageAllocator::kNoAccessWillJitLater) {
    access = PageAllocator::kNoAccess;
  }
#endif
  return base::OS::Allocate(hint, size, alignment,
                            static_cast<base::OS::MemoryPermission>(access));
}

bool PageAllocator::FreePages(void* address, size_t size) {
  return base::OS::Free(address, size);
}

bool PageAllocator::ReleasePages(void* address, size_t size, size_t new_size) {
  DCHECK_LT(new_size, size);
  return base::OS::Release(reinterpret_cast<uint8_t*>(address) + new_size,
                           size - new_size);
}

bool PageAllocator::SetPermissions(void* address, size_t size,
                                   PageAllocator::Permission access) {
  return base::OS::SetPermissions(
      address, size, static_cast<base::OS::MemoryPermission>(access));
}

bool PageAllocator::DiscardSystemPages(void* address, size_t size) {
  return base::OS::DiscardSystemPages(address, size);
}

}  // namespace base
}  // namespace v8
