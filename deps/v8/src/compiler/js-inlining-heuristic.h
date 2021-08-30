// Copyright 2015 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_COMPILER_JS_INLINING_HEURISTIC_H_
#define V8_COMPILER_JS_INLINING_HEURISTIC_H_

#include "src/compiler/js-inlining.h"

namespace v8 {
namespace internal {
namespace compiler {

class JSInliningHeuristic final : public AdvancedReducer {
 public:
  enum Mode { kJSOnly, kWasmOnly };

  JSInliningHeuristic(Editor* editor, Zone* local_zone,
                      OptimizedCompilationInfo* info, JSGraph* jsgraph,
                      JSHeapBroker* broker,
                      SourcePositionTable* source_positions, Mode mode)
      : AdvancedReducer(editor),
        inliner_(editor, local_zone, info, jsgraph, broker, source_positions),
        candidates_(local_zone),
        seen_(local_zone),
        source_positions_(source_positions),
        jsgraph_(jsgraph),
        broker_(broker),
        mode_(mode),
        max_inlined_bytecode_size_(
            ScaleInliningSize(FLAG_max_inlined_bytecode_size, broker)),
        max_inlined_bytecode_size_cumulative_(ScaleInliningSize(
            FLAG_max_inlined_bytecode_size_cumulative, broker)),
        max_inlined_bytecode_size_absolute_(ScaleInliningSize(
            FLAG_max_inlined_bytecode_size_absolute, broker)) {}

  const char* reducer_name() const override { return "JSInliningHeuristic"; }

  Reduction Reduce(Node* node) final;

  // Processes the list of candidates gathered while the reducer was running,
  // and inlines call sites that the heuristic determines to be important.
  void Finalize() final;

  int total_inlined_bytecode_size() const {
    return total_inlined_bytecode_size_;
  }

 private:
  // This limit currently matches what the old compiler did. We may want to
  // re-evaluate and come up with a proper limit for TurboFan.
  static const int kMaxCallPolymorphism = 4;

  struct Candidate {
    base::Optional<JSFunctionRef> functions[kMaxCallPolymorphism];
    // In the case of polymorphic inlining, this tells if each of the
    // functions could be inlined.
    bool can_inline_function[kMaxCallPolymorphism];
    // Strong references to bytecode to ensure it is not flushed from SFI
    // while choosing inlining candidates.
    base::Optional<BytecodeArrayRef> bytecode[kMaxCallPolymorphism];
    // TODO(2206): For now polymorphic inlining is treated orthogonally to
    // inlining based on SharedFunctionInfo. This should be unified and the
    // above array should be switched to SharedFunctionInfo instead. Currently
    // we use {num_functions == 1 && functions[0].is_null()} as an indicator.
    base::Optional<SharedFunctionInfoRef> shared_info;
    int num_functions;
    Node* node = nullptr;     // The call site at which to inline.
    CallFrequency frequency;  // Relative frequency of this call site.
    int total_size = 0;
  };

  // Comparator for candidates.
  struct CandidateCompare {
    bool operator()(const Candidate& left, const Candidate& right) const;
  };

  // Candidates are kept in a sorted set of unique candidates.
  using Candidates = ZoneSet<Candidate, CandidateCompare>;

  static int ScaleInliningSize(int value, JSHeapBroker* broker);

  // Dumps candidates to console.
  void PrintCandidates();
  Reduction InlineCandidate(Candidate const& candidate, bool small_function);
  void CreateOrReuseDispatch(Node* node, Node* callee,
                             Candidate const& candidate, Node** if_successes,
                             Node** calls, Node** inputs, int input_count);
  bool TryReuseDispatch(Node* node, Node* callee, Node** if_successes,
                        Node** calls, Node** inputs, int input_count);
  enum StateCloneMode { kCloneState, kChangeInPlace };
  FrameState DuplicateFrameStateAndRename(FrameState frame_state, Node* from,
                                          Node* to, StateCloneMode mode);
  Node* DuplicateStateValuesAndRename(Node* state_values, Node* from, Node* to,
                                      StateCloneMode mode);
  Candidate CollectFunctions(Node* node, int functions_size);

  CommonOperatorBuilder* common() const;
  Graph* graph() const;
  JSGraph* jsgraph() const { return jsgraph_; }
  // TODO(neis): Make heap broker a component of JSGraph?
  JSHeapBroker* broker() const { return broker_; }
  Isolate* isolate() const { return jsgraph_->isolate(); }
  SimplifiedOperatorBuilder* simplified() const;
  Mode mode() const { return mode_; }

  JSInliner inliner_;
  Candidates candidates_;
  ZoneSet<NodeId> seen_;
  SourcePositionTable* source_positions_;
  JSGraph* const jsgraph_;
  JSHeapBroker* const broker_;
  int total_inlined_bytecode_size_ = 0;
  const Mode mode_;
  const int max_inlined_bytecode_size_;
  const int max_inlined_bytecode_size_cumulative_;
  const int max_inlined_bytecode_size_absolute_;
};

}  // namespace compiler
}  // namespace internal
}  // namespace v8

#endif  // V8_COMPILER_JS_INLINING_HEURISTIC_H_
