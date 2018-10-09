// Copyright 2010 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_PROFILER_UNBOUND_QUEUE_H_
#define V8_PROFILER_UNBOUND_QUEUE_H_

#include "src/allocation.h"
#include "src/base/atomicops.h"

namespace v8 {
namespace internal {


// Lock-free unbound queue for small records.  Intended for
// transferring small records between a Single producer and a Single
// consumer. Doesn't have restrictions on the number of queued
// elements, so producer never blocks.  Implemented after Herb
// Sutter's article:
// http://www.ddj.com/high-performance-computing/210604448
template<typename Record>
class UnboundQueue BASE_EMBEDDED {
 public:
  inline UnboundQueue();
  inline ~UnboundQueue();

  V8_INLINE bool Dequeue(Record* rec);
  V8_INLINE void Enqueue(const Record& rec);
  V8_INLINE bool IsEmpty() const;
  V8_INLINE Record* Peek() const;

 private:
  V8_INLINE void DeleteFirst();

  struct Node;

  Node* first_;
  base::AtomicWord divider_;  // Node*
  base::AtomicWord last_;     // Node*

  DISALLOW_COPY_AND_ASSIGN(UnboundQueue);
};


}  // namespace internal
}  // namespace v8

#endif  // V8_PROFILER_UNBOUND_QUEUE_H_
