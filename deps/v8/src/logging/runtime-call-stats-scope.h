// Copyright 2021 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_LOGGING_RUNTIME_CALL_STATS_SCOPE_H_
#define V8_LOGGING_RUNTIME_CALL_STATS_SCOPE_H_

#include <memory>

#include "src/execution/isolate.h"
#include "src/logging/counters.h"
#include "src/logging/runtime-call-stats.h"
#include "src/logging/tracing-flags.h"

namespace v8 {
namespace internal {

#ifdef V8_RUNTIME_CALL_STATS

#define RCS_SCOPE(...) \
  v8::internal::RuntimeCallTimerScope rcs_timer_scope(__VA_ARGS__)

RuntimeCallTimerScope::RuntimeCallTimerScope(Isolate* isolate,
                                             RuntimeCallCounterId counter_id) {
  if (V8_LIKELY(!TracingFlags::is_runtime_stats_enabled())) return;
  stats_ = isolate->counters()->runtime_call_stats();
  stats_->Enter(&timer_, counter_id);
}

#else  // RUNTIME_CALL_STATS

#define RCS_SCOPE(...)

#endif  // defined(V8_RUNTIME_CALL_STATS)

}  // namespace internal
}  // namespace v8

#endif  // V8_LOGGING_RUNTIME_CALL_STATS_SCOPE_H_
