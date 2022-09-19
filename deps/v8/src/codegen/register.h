// Copyright 2018 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_CODEGEN_REGISTER_H_
#define V8_CODEGEN_REGISTER_H_

#include "src/codegen/register-arch.h"
#include "src/codegen/reglist.h"

namespace v8 {
namespace internal {

constexpr int AddArgumentPaddingSlots(int argument_count) {
  return argument_count + ArgumentPaddingSlots(argument_count);
}

constexpr bool ShouldPadArguments(int argument_count) {
  return ArgumentPaddingSlots(argument_count) != 0;
}

#ifdef DEBUG
template <typename... RegTypes,
          // All arguments must be either Register or DoubleRegister.
          typename = typename std::enable_if_t<
              std::conjunction_v<std::is_same<Register, RegTypes>...> ||
              std::conjunction_v<std::is_same<DoubleRegister, RegTypes>...>>>
inline constexpr bool AreAliased(RegTypes... regs) {
  int num_different_regs = RegListBase{regs...}.Count();
  int num_given_regs = (... + (regs.is_valid() ? 1 : 0));
  return num_different_regs < num_given_regs;
}
#endif

}  // namespace internal
}  // namespace v8

#endif  // V8_CODEGEN_REGISTER_H_
