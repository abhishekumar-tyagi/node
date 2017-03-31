// Copyright 2016 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_INTERPRETER_BYTECODE_PEEPHOLE_TABLE_H_
#define V8_INTERPRETER_BYTECODE_PEEPHOLE_TABLE_H_

#include "src/interpreter/bytecodes.h"

namespace v8 {
namespace internal {
namespace interpreter {

#define PEEPHOLE_NON_JUMP_ACTION_LIST(V)              \
  V(DefaultAction)                                    \
  V(UpdateLastAction)                                 \
  V(UpdateLastIfSourceInfoPresentAction)              \
  V(ElideCurrentAction)                               \
  V(ElideCurrentIfOperand0MatchesAction)              \
  V(ElideLastAction)                                  \
  V(ChangeBytecodeAction)                             \
  V(TransformLdaSmiBinaryOpToBinaryOpWithSmiAction)   \
  V(TransformLdaZeroBinaryOpToBinaryOpWithZeroAction) \
  V(TransformEqualityWithNullOrUndefinedAction)

#define PEEPHOLE_JUMP_ACTION_LIST(V) \
  V(DefaultJumpAction)               \
  V(UpdateLastJumpAction)            \
  V(ChangeJumpBytecodeAction)        \
  V(ElideLastBeforeJumpAction)

#define PEEPHOLE_ACTION_LIST(V)    \
  PEEPHOLE_NON_JUMP_ACTION_LIST(V) \
  PEEPHOLE_JUMP_ACTION_LIST(V)

// Actions to take when a pair of bytes is encountered. A handler
// exists for each action.
enum class PeepholeAction : uint8_t {
#define DECLARE_PEEPHOLE_ACTION(Action) k##Action,
  PEEPHOLE_ACTION_LIST(DECLARE_PEEPHOLE_ACTION)
#undef DECLARE_PEEPHOLE_ACTION
};

// Tuple of action to take when pair of bytecodes is encountered and
// optional data to invoke handler with.
struct PeepholeActionAndData final {
  // Action to take when tuple of bytecodes encountered.
  PeepholeAction action;

  // Replacement bytecode (if valid).
  Bytecode bytecode;
};

// Lookup table for matching pairs of bytecodes to peephole optimization
// actions. The contents of the table are generated by mkpeephole.cc.
struct PeepholeActionTable final {
 public:
  static const PeepholeActionAndData* Lookup(Bytecode last, Bytecode current);

 private:
  static const size_t kNumberOfBytecodes =
      static_cast<size_t>(Bytecode::kLast) + 1;

  static const PeepholeActionAndData row_data_[][kNumberOfBytecodes];
  static const PeepholeActionAndData* const row_[kNumberOfBytecodes];
};

}  // namespace interpreter
}  // namespace internal
}  // namespace v8

#endif  // V8_INTERPRETER_BYTECODE_PEEPHOLE_TABLE_H_
