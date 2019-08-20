// Copyright 2018 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_BUILTINS_BUILTINS_LAZY_GEN_H_
#define V8_BUILTINS_BUILTINS_LAZY_GEN_H_

#include "src/codegen/code-stub-assembler.h"

namespace v8 {
namespace internal {

class LazyBuiltinsAssembler : public CodeStubAssembler {
 public:
  using Descriptor = JSTrampolineDescriptor;

  explicit LazyBuiltinsAssembler(compiler::CodeAssemblerState* state)
      : CodeStubAssembler(state) {}

  void GenerateTailCallToJSCode(TNode<Code> code, TNode<JSFunction> function);

  void GenerateTailCallToReturnedCode(Runtime::FunctionId function_id,
                                      TNode<JSFunction> function);
  void TailCallRuntimeIfMarkerEquals(TNode<Smi> marker,
                                     OptimizationMarker expected_marker,
                                     Runtime::FunctionId function_id,
                                     TNode<JSFunction> function);

  void MaybeTailCallOptimizedCodeSlot(TNode<JSFunction> function,
                                      TNode<FeedbackVector> feedback_vector);
  void CompileLazy(TNode<JSFunction> function);
};

}  // namespace internal
}  // namespace v8

#endif  // V8_BUILTINS_BUILTINS_LAZY_GEN_H_
