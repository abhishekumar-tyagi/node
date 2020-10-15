// Copyright 2017 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_COMPILER_ALLOCATION_BUILDER_H_
#define V8_COMPILER_ALLOCATION_BUILDER_H_

#include "src/compiler/js-graph.h"
#include "src/compiler/node.h"
#include "src/compiler/simplified-operator.h"

namespace v8 {
namespace internal {
namespace compiler {

// A helper class to construct inline allocations on the simplified operator
// level. This keeps track of the effect chain for initial stores on a newly
// allocated object and also provides helpers for commonly allocated objects.
class AllocationBuilder final {
 public:
  AllocationBuilder(JSGraph* jsgraph, Node* effect, Node* control)
      : jsgraph_(jsgraph),
        allocation_(nullptr),
        effect_(effect),
        control_(control) {}

  // Primitive allocation of static size.
  void Allocate(int size, AllocationType allocation = AllocationType::kYoung,
                Type type = Type::Any()) {
    DCHECK_LE(size, kMaxRegularHeapObjectSize);
    effect_ = graph()->NewNode(
        common()->BeginRegion(RegionObservability::kNotObservable), effect_);
    allocation_ =
        graph()->NewNode(simplified()->Allocate(type, allocation),
                         jsgraph()->Constant(size), effect_, control_);
    effect_ = allocation_;
  }

  // Primitive store into a field.
  void Store(const FieldAccess& access, Node* value) {
    effect_ = graph()->NewNode(simplified()->StoreField(access), allocation_,
                               value, effect_, control_);
  }

  // Primitive store into an element.
  void Store(ElementAccess const& access, Node* index, Node* value) {
    effect_ = graph()->NewNode(simplified()->StoreElement(access), allocation_,
                               index, value, effect_, control_);
  }

  // Compound allocation of a context.
  inline void AllocateContext(int variadic_part_length, MapRef map);

  // Compound allocation of a FixedArray.
  inline void AllocateArray(int length, MapRef map,
                            AllocationType allocation = AllocationType::kYoung);

  // Compound allocation of a SloppyArgumentsElements
  inline void AllocateSloppyArgumentElements(
      int length, MapRef map,
      AllocationType allocation = AllocationType::kYoung);

  // Compound store of a constant into a field.
  void Store(const FieldAccess& access, const ObjectRef& value) {
    Store(access, jsgraph()->Constant(value));
  }

  void FinishAndChange(Node* node) {
    NodeProperties::SetType(allocation_, NodeProperties::GetType(node));
    node->ReplaceInput(0, allocation_);
    node->ReplaceInput(1, effect_);
    node->TrimInputCount(2);
    NodeProperties::ChangeOp(node, common()->FinishRegion());
  }

  Node* Finish() {
    return graph()->NewNode(common()->FinishRegion(), allocation_, effect_);
  }

 protected:
  JSGraph* jsgraph() { return jsgraph_; }
  Isolate* isolate() const { return jsgraph_->isolate(); }
  Graph* graph() { return jsgraph_->graph(); }
  CommonOperatorBuilder* common() { return jsgraph_->common(); }
  SimplifiedOperatorBuilder* simplified() { return jsgraph_->simplified(); }

 private:
  JSGraph* const jsgraph_;
  Node* allocation_;
  Node* effect_;
  Node* control_;
};

}  // namespace compiler
}  // namespace internal
}  // namespace v8

#endif  // V8_COMPILER_ALLOCATION_BUILDER_H_
