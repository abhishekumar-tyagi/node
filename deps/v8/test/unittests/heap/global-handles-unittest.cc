// Copyright 2013 the V8 project authors. All rights reserved.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials provided
//       with the distribution.
//     * Neither the name of Google Inc. nor the names of its
//       contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "src/handles/global-handles.h"

#include "include/v8-function.h"
#include "src/api/api-inl.h"
#include "src/execution/isolate.h"
#include "src/flags/flags.h"
#include "src/heap/factory.h"
#include "src/heap/heap-inl.h"
#include "src/objects/objects-inl.h"
#include "test/unittests/heap/heap-utils.h"
#include "test/unittests/test-utils.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace v8 {
namespace internal {

namespace {

struct TracedReferenceWrapper {
  v8::TracedReference<v8::Object> handle;
};

START_ALLOW_USE_DEPRECATED()

// Empty v8::EmbedderHeapTracer that never keeps objects alive on Scavenge. See
// |IsRootForNonTracingGC|.
class NonRootingEmbedderHeapTracer final : public v8::EmbedderHeapTracer {
 public:
  NonRootingEmbedderHeapTracer() = default;

  void RegisterV8References(
      const std::vector<std::pair<void*, void*>>& embedder_fields) final {}
  bool AdvanceTracing(double deadline_in_ms) final { return true; }
  bool IsTracingDone() final { return true; }
  void TracePrologue(TraceFlags) final {}
  void TraceEpilogue(TraceSummary*) final {}
  void EnterFinalPause(EmbedderStackState) final {}

  bool IsRootForNonTracingGC(
      const v8::TracedReference<v8::Value>& handle) final {
    return false;
  }

  void ResetHandleInNonTracingGC(
      const v8::TracedReference<v8::Value>& handle) final {
    for (auto* wrapper : wrappers_) {
      if (wrapper->handle == handle) {
        wrapper->handle.Reset();
      }
    }
  }

  void Register(TracedReferenceWrapper* wrapper) {
    wrappers_.push_back(wrapper);
  }

 private:
  std::vector<TracedReferenceWrapper*> wrappers_;
};

END_ALLOW_USE_DEPRECATED()

void SimpleCallback(const v8::FunctionCallbackInfo<v8::Value>& info) {
  v8::Isolate* isolate = info.GetIsolate();
  info.GetReturnValue().Set(v8::Number::New(isolate, 0));
}

struct FlagAndGlobal {
  bool flag;
  v8::Global<v8::Object> handle;
};

void ResetHandleAndSetFlag(const v8::WeakCallbackInfo<FlagAndGlobal>& data) {
  data.GetParameter()->handle.Reset();
  data.GetParameter()->flag = true;
}

template <typename HandleContainer>
void ConstructJSObject(v8::Isolate* isolate, v8::Local<v8::Context> context,
                       HandleContainer* flag_and_persistent) {
  v8::HandleScope handle_scope(isolate);
  v8::Local<v8::Object> object(v8::Object::New(isolate));
  CHECK(!object.IsEmpty());
  flag_and_persistent->handle.Reset(isolate, object);
  CHECK(!flag_and_persistent->handle.IsEmpty());
}

void ConstructJSObject(v8::Isolate* isolate, v8::Global<v8::Object>* global) {
  v8::HandleScope scope(isolate);
  v8::Local<v8::Object> object(v8::Object::New(isolate));
  CHECK(!object.IsEmpty());
  *global = v8::Global<v8::Object>(isolate, object);
  CHECK(!global->IsEmpty());
}

void ConstructJSObject(v8::Isolate* isolate,
                       v8::TracedReference<v8::Object>* handle) {
  v8::HandleScope scope(isolate);
  v8::Local<v8::Object> object(v8::Object::New(isolate));
  CHECK(!object.IsEmpty());
  *handle = v8::TracedReference<v8::Object>(isolate, object);
  CHECK(!handle->IsEmpty());
}

template <typename HandleContainer>
void ConstructJSApiObject(v8::Isolate* isolate, v8::Local<v8::Context> context,
                          HandleContainer* flag_and_persistent) {
  v8::HandleScope handle_scope(isolate);
  v8::Local<v8::FunctionTemplate> fun =
      v8::FunctionTemplate::New(isolate, SimpleCallback);
  v8::Local<v8::Object> object = fun->GetFunction(context)
                                     .ToLocalChecked()
                                     ->NewInstance(context)
                                     .ToLocalChecked();
  CHECK(!object.IsEmpty());
  flag_and_persistent->handle.Reset(isolate, object);
  CHECK(!flag_and_persistent->handle.IsEmpty());
}

enum class SurvivalMode { kSurvives, kDies };

template <typename ConstructFunction, typename ModifierFunction,
          typename GCFunction>
void WeakHandleTest(v8::Isolate* isolate, ConstructFunction construct_function,
                    ModifierFunction modifier_function, GCFunction gc_function,
                    SurvivalMode survives) {
  v8::HandleScope scope(isolate);
  v8::Local<v8::Context> context = v8::Context::New(isolate);
  v8::Context::Scope context_scope(context);

  FlagAndGlobal fp;
  construct_function(isolate, context, &fp);
  CHECK(IsNewObjectInCorrectGeneration(isolate, fp.handle));
  fp.handle.SetWeak(&fp, &ResetHandleAndSetFlag,
                    v8::WeakCallbackType::kParameter);
  fp.flag = false;
  modifier_function(&fp);
  gc_function();
  CHECK_IMPLIES(survives == SurvivalMode::kSurvives, !fp.flag);
  CHECK_IMPLIES(survives == SurvivalMode::kDies, fp.flag);
}

void EmptyWeakCallback(const v8::WeakCallbackInfo<void>& data) {}

class GlobalHandlesTest : public TestWithContext {
 protected:
  template <typename ConstructFunction, typename ModifierFunction>
  void TracedReferenceTestWithScavenge(ConstructFunction construct_function,
                                       ModifierFunction modifier_function,
                                       SurvivalMode survives) {
    v8::Isolate* isolate = v8_isolate();
    v8::HandleScope scope(isolate);
    v8::Local<v8::Context> context = v8::Context::New(isolate);
    v8::Context::Scope context_scope(context);

    NonRootingEmbedderHeapTracer tracer;
    TemporaryEmbedderHeapTracerScope tracer_scope(isolate, &tracer);

    auto fp = std::make_unique<TracedReferenceWrapper>();
    tracer.Register(fp.get());
    construct_function(isolate, context, fp.get());
    CHECK(IsNewObjectInCorrectGeneration(isolate, fp->handle));
    modifier_function(fp.get());
    CollectGarbage(i::NEW_SPACE);
    // Scavenge clear properly resets the original handle, so we can check the
    // handle directly here.
    CHECK_IMPLIES(survives == SurvivalMode::kSurvives, !fp->handle.IsEmpty());
    CHECK_IMPLIES(survives == SurvivalMode::kDies, fp->handle.IsEmpty());
  }
};

}  // namespace

TEST_F(GlobalHandlesTest, EternalHandles) {
  Isolate* isolate = i_isolate();
  v8::Isolate* v8_isolate = reinterpret_cast<v8::Isolate*>(isolate);
  EternalHandles* eternal_handles = isolate->eternal_handles();

  // Create a number of handles that will not be on a block boundary
  const int kArrayLength = 2048 - 1;
  int indices[kArrayLength];
  v8::Eternal<v8::Value> eternals[kArrayLength];

  CHECK_EQ(0, eternal_handles->handles_count());
  for (int i = 0; i < kArrayLength; i++) {
    indices[i] = -1;
    HandleScope scope(isolate);
    v8::Local<v8::Object> object = v8::Object::New(v8_isolate);
    object
        ->Set(v8_isolate->GetCurrentContext(), i,
              v8::Integer::New(v8_isolate, i))
        .FromJust();
    // Create with internal api
    eternal_handles->Create(isolate, *v8::Utils::OpenHandle(*object),
                            &indices[i]);
    // Create with external api
    CHECK(eternals[i].IsEmpty());
    eternals[i].Set(v8_isolate, object);
    CHECK(!eternals[i].IsEmpty());
  }

  CollectAllAvailableGarbage();

  for (int i = 0; i < kArrayLength; i++) {
    for (int j = 0; j < 2; j++) {
      HandleScope scope(isolate);
      v8::Local<v8::Value> local;
      if (j == 0) {
        // Test internal api
        local = v8::Utils::ToLocal(eternal_handles->Get(indices[i]));
      } else {
        // Test external api
        local = eternals[i].Get(v8_isolate);
      }
      v8::Local<v8::Object> object = v8::Local<v8::Object>::Cast(local);
      v8::Local<v8::Value> value =
          object->Get(v8_isolate->GetCurrentContext(), i).ToLocalChecked();
      CHECK(value->IsInt32());
      CHECK_EQ(i,
               value->Int32Value(v8_isolate->GetCurrentContext()).FromJust());
    }
  }

  CHECK_EQ(2 * kArrayLength, eternal_handles->handles_count());

  // Create an eternal via the constructor
  {
    HandleScope scope(isolate);
    v8::Local<v8::Object> object = v8::Object::New(v8_isolate);
    v8::Eternal<v8::Object> eternal(v8_isolate, object);
    CHECK(!eternal.IsEmpty());
    CHECK(object == eternal.Get(v8_isolate));
  }

  CHECK_EQ(2 * kArrayLength + 1, eternal_handles->handles_count());
}

TEST_F(GlobalHandlesTest, PersistentBaseGetLocal) {
  v8::Isolate* isolate = v8_isolate();

  v8::HandleScope scope(isolate);
  v8::Local<v8::Object> o = v8::Object::New(isolate);
  CHECK(!o.IsEmpty());
  v8::Persistent<v8::Object> p(isolate, o);
  CHECK(o == p.Get(isolate));
  CHECK(v8::Local<v8::Object>::New(isolate, p) == p.Get(isolate));

  v8::Global<v8::Object> g(isolate, o);
  CHECK(o == g.Get(isolate));
  CHECK(v8::Local<v8::Object>::New(isolate, g) == g.Get(isolate));
}

TEST_F(GlobalHandlesTest, WeakPersistentSmi) {
  v8::Isolate* isolate = v8_isolate();

  v8::HandleScope scope(isolate);
  v8::Local<v8::Number> n = v8::Number::New(isolate, 0);
  v8::Global<v8::Number> g(isolate, n);

  // Should not crash.
  g.SetWeak<void>(nullptr, &EmptyWeakCallback,
                  v8::WeakCallbackType::kParameter);
}

START_ALLOW_USE_DEPRECATED()

TEST_F(GlobalHandlesTest, PhantomHandlesWithoutCallbacks) {
  v8::Isolate* isolate = v8_isolate();

  v8::Global<v8::Object> g1, g2;
  {
    v8::HandleScope scope(isolate);
    g1.Reset(isolate, v8::Object::New(isolate));
    g1.SetWeak();
    g2.Reset(isolate, v8::Object::New(isolate));
    g2.SetWeak();
  }
  CHECK(!g1.IsEmpty());
  CHECK(!g2.IsEmpty());
  CollectAllAvailableGarbage();
  CHECK(g1.IsEmpty());
  CHECK(g2.IsEmpty());
}

TEST_F(GlobalHandlesTest, WeakHandleToUnmodifiedJSObjectDiesOnScavenge) {
  if (v8_flags.single_generation) return;

  WeakHandleTest(
      v8_isolate(), &ConstructJSObject<FlagAndGlobal>, [](FlagAndGlobal* fp) {},
      [this]() { CollectGarbage(i::NEW_SPACE); }, SurvivalMode::kDies);
}

TEST_F(GlobalHandlesTest, TracedReferenceToUnmodifiedJSObjectSurvivesScavenge) {
  if (v8_flags.single_generation) return;

  ManualGCScope manual_gc(i_isolate());
  TracedReferenceTestWithScavenge(
      &ConstructJSObject<TracedReferenceWrapper>,
      [](TracedReferenceWrapper* fp) {}, SurvivalMode::kSurvives);
}

TEST_F(GlobalHandlesTest, WeakHandleToUnmodifiedJSObjectDiesOnMarkCompact) {
  WeakHandleTest(
      v8_isolate(), &ConstructJSObject<FlagAndGlobal>, [](FlagAndGlobal* fp) {},
      [this]() { CollectAllGarbage(); }, SurvivalMode::kDies);
}

TEST_F(GlobalHandlesTest,
       WeakHandleToUnmodifiedJSObjectSurvivesMarkCompactWhenInHandle) {
  WeakHandleTest(
      v8_isolate(), &ConstructJSObject<FlagAndGlobal>,
      [this](FlagAndGlobal* fp) {
        v8::Local<v8::Object> handle =
            v8::Local<v8::Object>::New(v8_isolate(), fp->handle);
        USE(handle);
      },
      [this]() { CollectAllGarbage(); }, SurvivalMode::kSurvives);
}

TEST_F(GlobalHandlesTest, WeakHandleToUnmodifiedJSApiObjectDiesOnScavenge) {
  if (v8_flags.single_generation) return;

  WeakHandleTest(
      v8_isolate(), &ConstructJSApiObject<FlagAndGlobal>,
      [](FlagAndGlobal* fp) {}, [this]() { CollectGarbage(i::NEW_SPACE); },
      SurvivalMode::kDies);
}

TEST_F(GlobalHandlesTest,
       TracedReferenceToUnmodifiedJSApiObjectDiesOnScavenge) {
  if (v8_flags.single_generation) return;

  ManualGCScope manual_gc(i_isolate());
  TracedReferenceTestWithScavenge(
      &ConstructJSApiObject<TracedReferenceWrapper>,
      [](TracedReferenceWrapper* fp) {}, SurvivalMode::kDies);
}

TEST_F(GlobalHandlesTest,
       TracedReferenceToJSApiObjectWithIdentityHashSurvivesScavenge) {
  if (v8_flags.single_generation) return;

  ManualGCScope manual_gc(i_isolate());
  Isolate* isolate = i_isolate();
  HandleScope scope(isolate);
  Handle<JSWeakMap> weakmap = isolate->factory()->NewJSWeakMap();

  TracedReferenceTestWithScavenge(
      &ConstructJSApiObject<TracedReferenceWrapper>,
      [this, &weakmap, isolate](TracedReferenceWrapper* fp) {
        v8::HandleScope scope(v8_isolate());
        Handle<JSReceiver> key =
            Utils::OpenHandle(*fp->handle.Get(v8_isolate()));
        Handle<Smi> smi(Smi::FromInt(23), isolate);
        int32_t hash = key->GetOrCreateHash(isolate).value();
        JSWeakCollection::Set(weakmap, key, smi, hash);
      },
      SurvivalMode::kSurvives);
}

TEST_F(GlobalHandlesTest,
       WeakHandleToUnmodifiedJSApiObjectSurvivesScavengeWhenInHandle) {
  if (v8_flags.single_generation) return;

  WeakHandleTest(
      v8_isolate(), &ConstructJSApiObject<FlagAndGlobal>,
      [this](FlagAndGlobal* fp) {
        v8::Local<v8::Object> handle =
            v8::Local<v8::Object>::New(v8_isolate(), fp->handle);
        USE(handle);
      },
      [this]() { CollectGarbage(i::NEW_SPACE); }, SurvivalMode::kSurvives);
}

TEST_F(GlobalHandlesTest, WeakHandleToUnmodifiedJSApiObjectDiesOnMarkCompact) {
  WeakHandleTest(
      v8_isolate(), &ConstructJSApiObject<FlagAndGlobal>,
      [](FlagAndGlobal* fp) {}, [this]() { CollectAllGarbage(); },
      SurvivalMode::kDies);
}

TEST_F(GlobalHandlesTest,
       WeakHandleToUnmodifiedJSApiObjectSurvivesMarkCompactWhenInHandle) {
  WeakHandleTest(
      v8_isolate(), &ConstructJSApiObject<FlagAndGlobal>,
      [this](FlagAndGlobal* fp) {
        v8::Local<v8::Object> handle =
            v8::Local<v8::Object>::New(v8_isolate(), fp->handle);
        USE(handle);
      },
      [this]() { CollectAllGarbage(); }, SurvivalMode::kSurvives);
}

TEST_F(GlobalHandlesTest,
       TracedReferenceToJSApiObjectWithModifiedMapSurvivesScavenge) {
  if (v8_flags.single_generation) return;

  v8::Isolate* isolate = v8_isolate();

  TracedReference<v8::Object> handle;
  {
    v8::HandleScope scope(isolate);
    // Create an API object which does not have the same map as constructor.
    auto function_template = FunctionTemplate::New(isolate);
    auto instance_t = function_template->InstanceTemplate();
    instance_t->Set(isolate, "a", v8::Number::New(isolate, 10));
    auto function =
        function_template->GetFunction(v8_context()).ToLocalChecked();
    auto i = function->NewInstance(v8_context()).ToLocalChecked();
    handle.Reset(isolate, i);
  }
  CollectGarbage(i::NEW_SPACE);
  CHECK(!handle.IsEmpty());
}

TEST_F(GlobalHandlesTest,
       TracedReferenceTOJsApiObjectWithElementsSurvivesScavenge) {
  if (v8_flags.single_generation) return;

  v8::Isolate* isolate = v8_isolate();

  TracedReference<v8::Object> handle;
  {
    v8::HandleScope scope(isolate);

    // Create an API object which has elements.
    auto function_template = FunctionTemplate::New(isolate);
    auto instance_t = function_template->InstanceTemplate();
    instance_t->Set(isolate, "1", v8::Number::New(isolate, 10));
    instance_t->Set(isolate, "2", v8::Number::New(isolate, 10));
    auto function =
        function_template->GetFunction(v8_context()).ToLocalChecked();
    auto i = function->NewInstance(v8_context()).ToLocalChecked();
    handle.Reset(isolate, i);
  }
  CollectGarbage(i::NEW_SPACE);
  CHECK(!handle.IsEmpty());
}

namespace {

void ForceScavenge2(const v8::WeakCallbackInfo<FlagAndGlobal>& data) {
  data.GetParameter()->flag = true;
  YoungGC(data.GetIsolate());
}

void ForceScavenge1(const v8::WeakCallbackInfo<FlagAndGlobal>& data) {
  data.GetParameter()->handle.Reset();
  data.SetSecondPassCallback(ForceScavenge2);
}

void ForceMarkSweep2(const v8::WeakCallbackInfo<FlagAndGlobal>& data) {
  data.GetParameter()->flag = true;
  FullGC(data.GetIsolate());
}

void ForceMarkSweep1(const v8::WeakCallbackInfo<FlagAndGlobal>& data) {
  data.GetParameter()->handle.Reset();
  data.SetSecondPassCallback(ForceMarkSweep2);
}

}  // namespace

TEST_F(GlobalHandlesTest, GCFromWeakCallbacks) {
  v8::Isolate* isolate = v8_isolate();
  v8::HandleScope scope(isolate);
  v8::Local<v8::Context> context = v8::Context::New(isolate);
  v8::Context::Scope context_scope(context);

  if (v8_flags.single_generation) {
    FlagAndGlobal fp;
    ConstructJSApiObject(isolate, context, &fp);
    CHECK_IMPLIES(!v8_flags.single_generation,
                  !InYoungGeneration(isolate, fp.handle));
    fp.flag = false;
    fp.handle.SetWeak(&fp, &ForceMarkSweep1, v8::WeakCallbackType::kParameter);
    CollectAllGarbage();
    EmptyMessageQueues();
    CHECK(fp.flag);
    return;
  }

  static const int kNumberOfGCTypes = 2;
  using Callback = v8::WeakCallbackInfo<FlagAndGlobal>::Callback;
  Callback gc_forcing_callback[kNumberOfGCTypes] = {&ForceScavenge1,
                                                    &ForceMarkSweep1};

  using GCInvoker = std::function<void(void)>;
  GCInvoker invoke_gc[kNumberOfGCTypes] = {
      [this]() { CollectGarbage(i::NEW_SPACE); },
      [this]() { CollectAllGarbage(); }};

  for (int outer_gc = 0; outer_gc < kNumberOfGCTypes; outer_gc++) {
    for (int inner_gc = 0; inner_gc < kNumberOfGCTypes; inner_gc++) {
      FlagAndGlobal fp;
      ConstructJSApiObject(isolate, context, &fp);
      CHECK(InYoungGeneration(isolate, fp.handle));
      fp.flag = false;
      fp.handle.SetWeak(&fp, gc_forcing_callback[inner_gc],
                        v8::WeakCallbackType::kParameter);
      invoke_gc[outer_gc]();
      EmptyMessageQueues();
      CHECK(fp.flag);
    }
  }
}

namespace {

void SecondPassCallback(const v8::WeakCallbackInfo<FlagAndGlobal>& data) {
  data.GetParameter()->flag = true;
}

void FirstPassCallback(const v8::WeakCallbackInfo<FlagAndGlobal>& data) {
  data.GetParameter()->handle.Reset();
  data.SetSecondPassCallback(SecondPassCallback);
}

}  // namespace

TEST_F(GlobalHandlesTest, SecondPassPhantomCallbacks) {
  v8::Isolate* isolate = v8_isolate();
  v8::HandleScope scope(isolate);
  v8::Local<v8::Context> context = v8::Context::New(isolate);
  v8::Context::Scope context_scope(context);
  FlagAndGlobal fp;
  ConstructJSApiObject(isolate, context, &fp);
  fp.flag = false;
  fp.handle.SetWeak(&fp, FirstPassCallback, v8::WeakCallbackType::kParameter);
  CHECK(!fp.flag);
  CollectAllGarbage();
  CollectAllGarbage();
  CHECK(fp.flag);
}

TEST_F(GlobalHandlesTest, MoveStrongGlobal) {
  v8::Isolate* isolate = v8_isolate();
  v8::HandleScope scope(isolate);

  v8::Global<v8::Object>* global = new Global<v8::Object>();
  ConstructJSObject(isolate, global);
  CollectAllGarbage();
  v8::Global<v8::Object> global2(std::move(*global));
  delete global;
  CollectAllGarbage();
}

TEST_F(GlobalHandlesTest, MoveWeakGlobal) {
  v8::Isolate* isolate = v8_isolate();
  v8::HandleScope scope(isolate);

  v8::Global<v8::Object>* global = new Global<v8::Object>();
  ConstructJSObject(isolate, global);
  CollectAllGarbage();
  global->SetWeak();
  v8::Global<v8::Object> global2(std::move(*global));
  delete global;
  CollectAllGarbage();
}

TEST_F(GlobalHandlesTest, TotalSizeRegularNode) {
  v8::Isolate* isolate = v8_isolate();
  v8::HandleScope scope(isolate);

  v8::Global<v8::Object>* global = new Global<v8::Object>();
  CHECK_EQ(i_isolate()->global_handles()->TotalSize(), 0);
  CHECK_EQ(i_isolate()->global_handles()->UsedSize(), 0);
  ConstructJSObject(isolate, global);
  CHECK_GT(i_isolate()->global_handles()->TotalSize(), 0);
  CHECK_GT(i_isolate()->global_handles()->UsedSize(), 0);
  delete global;
  CHECK_GT(i_isolate()->global_handles()->TotalSize(), 0);
  CHECK_EQ(i_isolate()->global_handles()->UsedSize(), 0);
}

TEST_F(GlobalHandlesTest, TotalSizeTracedNode) {
  ManualGCScope manual_gc(i_isolate());
  v8::Isolate* isolate = v8_isolate();
  v8::HandleScope scope(isolate);

  v8::TracedReference<v8::Object>* handle = new TracedReference<v8::Object>();
  CHECK_EQ(i_isolate()->traced_handles()->total_size_bytes(), 0);
  CHECK_EQ(i_isolate()->traced_handles()->used_size_bytes(), 0);
  ConstructJSObject(isolate, handle);
  CHECK_GT(i_isolate()->traced_handles()->total_size_bytes(), 0);
  CHECK_GT(i_isolate()->traced_handles()->used_size_bytes(), 0);
  delete handle;
  CollectAllGarbage();
  CHECK_GT(i_isolate()->traced_handles()->total_size_bytes(), 0);
  CHECK_EQ(i_isolate()->traced_handles()->used_size_bytes(), 0);
}

}  // namespace internal
}  // namespace v8
