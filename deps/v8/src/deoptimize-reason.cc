// Copyright 2016 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/deoptimize-reason.h"

namespace v8 {
namespace internal {

std::ostream& operator<<(std::ostream& os, DeoptimizeReason reason) {
  switch (reason) {
#define DEOPTIMIZE_REASON(Name, message) \
  case DeoptimizeReason::k##Name:        \
    return os << #Name;
    DEOPTIMIZE_REASON_LIST(DEOPTIMIZE_REASON)
#undef DEOPTIMIZE_REASON
  }
  UNREACHABLE();
  return os;
}

size_t hash_value(DeoptimizeReason reason) {
  return static_cast<uint8_t>(reason);
}

char const* const DeoptimizeReasonToString(DeoptimizeReason reason) {
  static char const* kDeoptimizeReasonStrings[] = {
#define DEOPTIMIZE_REASON(Name, message) message,
      DEOPTIMIZE_REASON_LIST(DEOPTIMIZE_REASON)
#undef DEOPTIMIZE_REASON
  };
  size_t const index = static_cast<size_t>(reason);
  DCHECK_LT(index, arraysize(kDeoptimizeReasonStrings));
  return kDeoptimizeReasonStrings[index];
}

}  // namespace internal
}  // namespace v8
