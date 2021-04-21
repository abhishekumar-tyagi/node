// Copyright 2017 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/builtins/builtins-async-gen.h"
#include "src/builtins/builtins-utils-gen.h"
#include "src/builtins/builtins.h"
#include "src/codegen/code-factory.h"
#include "src/codegen/code-stub-assembler.h"
#include "src/execution/frames-inl.h"
#include "src/objects/js-generator.h"
#include "src/objects/js-promise.h"

namespace v8 {
namespace internal {

using compiler::Node;

namespace {

class AsyncGeneratorBuiltinsAssembler : public AsyncBuiltinsAssembler {
 public:
  explicit AsyncGeneratorBuiltinsAssembler(CodeAssemblerState* state)
      : AsyncBuiltinsAssembler(state) {}

  inline TNode<Smi> LoadGeneratorState(
      const TNode<JSGeneratorObject> generator) {
    return LoadObjectField<Smi>(generator,
                                JSGeneratorObject::kContinuationOffset);
  }

  inline TNode<BoolT> IsGeneratorStateClosed(const TNode<Smi> state) {
    return SmiEqual(state, SmiConstant(JSGeneratorObject::kGeneratorClosed));
  }
  inline TNode<BoolT> IsGeneratorClosed(
      const TNode<JSGeneratorObject> generator) {
    return IsGeneratorStateClosed(LoadGeneratorState(generator));
  }

  inline TNode<BoolT> IsGeneratorStateSuspended(const TNode<Smi> state) {
    return SmiGreaterThanOrEqual(state, SmiConstant(0));
  }

  inline TNode<BoolT> IsGeneratorSuspended(
      const TNode<JSGeneratorObject> generator) {
    return IsGeneratorStateSuspended(LoadGeneratorState(generator));
  }

  inline TNode<BoolT> IsGeneratorStateSuspendedAtStart(const TNode<Smi> state) {
    return SmiEqual(state, SmiConstant(0));
  }

  inline TNode<BoolT> IsGeneratorStateNotExecuting(const TNode<Smi> state) {
    return SmiNotEqual(state,
                       SmiConstant(JSGeneratorObject::kGeneratorExecuting));
  }
  inline TNode<BoolT> IsGeneratorNotExecuting(
      const TNode<JSGeneratorObject> generator) {
    return IsGeneratorStateNotExecuting(LoadGeneratorState(generator));
  }

  inline TNode<BoolT> IsGeneratorAwaiting(
      const TNode<JSGeneratorObject> generator) {
    TNode<Object> is_generator_awaiting =
        LoadObjectField(generator, JSAsyncGeneratorObject::kIsAwaitingOffset);
    return TaggedEqual(is_generator_awaiting, SmiConstant(1));
  }

  inline void SetGeneratorAwaiting(const TNode<JSGeneratorObject> generator) {
    CSA_ASSERT(this, Word32BinaryNot(IsGeneratorAwaiting(generator)));
    StoreObjectFieldNoWriteBarrier(
        generator, JSAsyncGeneratorObject::kIsAwaitingOffset, SmiConstant(1));
    CSA_ASSERT(this, IsGeneratorAwaiting(generator));
  }

  inline void SetGeneratorNotAwaiting(
      const TNode<JSGeneratorObject> generator) {
    CSA_ASSERT(this, IsGeneratorAwaiting(generator));
    StoreObjectFieldNoWriteBarrier(
        generator, JSAsyncGeneratorObject::kIsAwaitingOffset, SmiConstant(0));
    CSA_ASSERT(this, Word32BinaryNot(IsGeneratorAwaiting(generator)));
  }

  inline void CloseGenerator(const TNode<JSGeneratorObject> generator) {
    StoreObjectFieldNoWriteBarrier(
        generator, JSGeneratorObject::kContinuationOffset,
        SmiConstant(JSGeneratorObject::kGeneratorClosed));
  }

  inline TNode<HeapObject> LoadFirstAsyncGeneratorRequestFromQueue(
      const TNode<JSGeneratorObject> generator) {
    return LoadObjectField<HeapObject>(generator,
                                       JSAsyncGeneratorObject::kQueueOffset);
  }

  inline TNode<Smi> LoadResumeTypeFromAsyncGeneratorRequest(
      const TNode<AsyncGeneratorRequest> request) {
    return LoadObjectField<Smi>(request,
                                AsyncGeneratorRequest::kResumeModeOffset);
  }

  inline TNode<JSPromise> LoadPromiseFromAsyncGeneratorRequest(
      const TNode<AsyncGeneratorRequest> request) {
    return LoadObjectField<JSPromise>(request,
                                      AsyncGeneratorRequest::kPromiseOffset);
  }

  inline TNode<Object> LoadValueFromAsyncGeneratorRequest(
      const TNode<AsyncGeneratorRequest> request) {
    return LoadObjectField(request, AsyncGeneratorRequest::kValueOffset);
  }

  inline TNode<BoolT> IsAbruptResumeType(const TNode<Smi> resume_type) {
    return SmiNotEqual(resume_type, SmiConstant(JSGeneratorObject::kNext));
  }

  void AsyncGeneratorEnqueue(CodeStubArguments* args, TNode<Context> context,
                             TNode<Object> receiver, TNode<Object> value,
                             JSAsyncGeneratorObject::ResumeMode resume_mode,
                             const char* method_name);

  TNode<AsyncGeneratorRequest> TakeFirstAsyncGeneratorRequestFromQueue(
      TNode<JSAsyncGeneratorObject> generator);
  void AddAsyncGeneratorRequestToQueue(TNode<JSAsyncGeneratorObject> generator,
                                       TNode<AsyncGeneratorRequest> request);

  TNode<AsyncGeneratorRequest> AllocateAsyncGeneratorRequest(
      JSAsyncGeneratorObject::ResumeMode resume_mode,
      TNode<Object> resume_value, TNode<JSPromise> promise);

  // Shared implementation of the catchable and uncatchable variations of Await
  // for AsyncGenerators.
  template <typename Descriptor>
  void AsyncGeneratorAwait(bool is_catchable);
  void AsyncGeneratorAwaitResumeClosure(
      TNode<Context> context, TNode<Object> value,
      JSAsyncGeneratorObject::ResumeMode resume_mode);
};

// Shared implementation for the 3 Async Iterator protocol methods of Async
// Generators.
void AsyncGeneratorBuiltinsAssembler::AsyncGeneratorEnqueue(
    CodeStubArguments* args, TNode<Context> context, TNode<Object> receiver,
    TNode<Object> value, JSAsyncGeneratorObject::ResumeMode resume_mode,
    const char* method_name) {
  // AsyncGeneratorEnqueue produces a new Promise, and appends it to the list
  // of async generator requests to be executed. If the generator is not
  // presently executing, then this method will loop through, processing each
  // request from front to back.
  // This loop resides in AsyncGeneratorResumeNext.
  TNode<JSPromise> promise = NewJSPromise(context);

  Label if_receiverisincompatible(this, Label::kDeferred);
  GotoIf(TaggedIsSmi(receiver), &if_receiverisincompatible);
  GotoIfNot(HasInstanceType(CAST(receiver), JS_ASYNC_GENERATOR_OBJECT_TYPE),
            &if_receiverisincompatible);

  {
    Label done(this);
    const TNode<JSAsyncGeneratorObject> generator = CAST(receiver);
    const TNode<AsyncGeneratorRequest> req =
        AllocateAsyncGeneratorRequest(resume_mode, value, promise);

    AddAsyncGeneratorRequestToQueue(generator, req);

    // Let state be generator.[[AsyncGeneratorState]]
    // If state is not "executing", then
    //     Perform AsyncGeneratorResumeNext(Generator)
    // Check if the {receiver} is running or already closed.
    TNode<Smi> continuation = LoadGeneratorState(generator);

    GotoIf(SmiEqual(continuation,
                    SmiConstant(JSAsyncGeneratorObject::kGeneratorExecuting)),
           &done);

    CallBuiltin(Builtins::kAsyncGeneratorResumeNext, context, generator);

    Goto(&done);
    BIND(&done);
    args->PopAndReturn(promise);
  }

  BIND(&if_receiverisincompatible);
  {
    CallBuiltin(Builtins::kRejectPromise, context, promise,
                MakeTypeError(MessageTemplate::kIncompatibleMethodReceiver,
                              context, StringConstant(method_name), receiver),
                TrueConstant());
    args->PopAndReturn(promise);
  }
}

TNode<AsyncGeneratorRequest>
AsyncGeneratorBuiltinsAssembler::AllocateAsyncGeneratorRequest(
    JSAsyncGeneratorObject::ResumeMode resume_mode, TNode<Object> resume_value,
    TNode<JSPromise> promise) {
  TNode<HeapObject> request = Allocate(AsyncGeneratorRequest::kSize);
  StoreMapNoWriteBarrier(request, RootIndex::kAsyncGeneratorRequestMap);
  StoreObjectFieldNoWriteBarrier(request, AsyncGeneratorRequest::kNextOffset,
                                 UndefinedConstant());
  StoreObjectFieldNoWriteBarrier(request,
                                 AsyncGeneratorRequest::kResumeModeOffset,
                                 SmiConstant(resume_mode));
  StoreObjectFieldNoWriteBarrier(request, AsyncGeneratorRequest::kValueOffset,
                                 resume_value);
  StoreObjectFieldNoWriteBarrier(request, AsyncGeneratorRequest::kPromiseOffset,
                                 promise);
  StoreObjectFieldRoot(request, AsyncGeneratorRequest::kNextOffset,
                       RootIndex::kUndefinedValue);
  return CAST(request);
}

void AsyncGeneratorBuiltinsAssembler::AsyncGeneratorAwaitResumeClosure(
    TNode<Context> context, TNode<Object> value,
    JSAsyncGeneratorObject::ResumeMode resume_mode) {
  const TNode<JSAsyncGeneratorObject> generator =
      CAST(LoadContextElement(context, Context::EXTENSION_INDEX));

  SetGeneratorNotAwaiting(generator);

  CSA_SLOW_ASSERT(this, IsGeneratorSuspended(generator));

  // Remember the {resume_mode} for the {generator}.
  StoreObjectFieldNoWriteBarrier(generator,
                                 JSGeneratorObject::kResumeModeOffset,
                                 SmiConstant(resume_mode));

  CallStub(CodeFactory::ResumeGenerator(isolate()), context, value, generator);

  TailCallBuiltin(Builtins::kAsyncGeneratorResumeNext, context, generator);
}

template <typename Descriptor>
void AsyncGeneratorBuiltinsAssembler::AsyncGeneratorAwait(bool is_catchable) {
  TNode<JSAsyncGeneratorObject> async_generator_object =
      CAST(Parameter(Descriptor::kAsyncGeneratorObject));
  TNode<Object> value = CAST(Parameter(Descriptor::kValue));
  TNode<Context> context = CAST(Parameter(Descriptor::kContext));

  TNode<AsyncGeneratorRequest> request =
      CAST(LoadFirstAsyncGeneratorRequestFromQueue(async_generator_object));
  TNode<JSPromise> outer_promise = LoadObjectField<JSPromise>(
      request, AsyncGeneratorRequest::kPromiseOffset);

  SetGeneratorAwaiting(async_generator_object);
  Await(context, async_generator_object, value, outer_promise,
        AsyncGeneratorAwaitResolveSharedFunConstant(),
        AsyncGeneratorAwaitRejectSharedFunConstant(), is_catchable);
  Return(UndefinedConstant());
}

void AsyncGeneratorBuiltinsAssembler::AddAsyncGeneratorRequestToQueue(
    TNode<JSAsyncGeneratorObject> generator,
    TNode<AsyncGeneratorRequest> request) {
  TVARIABLE(HeapObject, var_current);
  Label empty(this), loop(this, &var_current), done(this);

  var_current = LoadObjectField<HeapObject>(
      generator, JSAsyncGeneratorObject::kQueueOffset);
  Branch(IsUndefined(var_current.value()), &empty, &loop);

  BIND(&empty);
  {
    StoreObjectField(generator, JSAsyncGeneratorObject::kQueueOffset, request);
    Goto(&done);
  }

  BIND(&loop);
  {
    Label loop_next(this), next_empty(this);
    TNode<AsyncGeneratorRequest> current = CAST(var_current.value());
    TNode<HeapObject> next = LoadObjectField<HeapObject>(
        current, AsyncGeneratorRequest::kNextOffset);

    Branch(IsUndefined(next), &next_empty, &loop_next);
    BIND(&next_empty);
    {
      StoreObjectField(current, AsyncGeneratorRequest::kNextOffset, request);
      Goto(&done);
    }

    BIND(&loop_next);
    {
      var_current = next;
      Goto(&loop);
    }
  }
  BIND(&done);
}

TNode<AsyncGeneratorRequest>
AsyncGeneratorBuiltinsAssembler::TakeFirstAsyncGeneratorRequestFromQueue(
    TNode<JSAsyncGeneratorObject> generator) {
  // Removes and returns the first AsyncGeneratorRequest from a
  // JSAsyncGeneratorObject's queue. Asserts that the queue is not empty.
  TNode<AsyncGeneratorRequest> request = LoadObjectField<AsyncGeneratorRequest>(
      generator, JSAsyncGeneratorObject::kQueueOffset);

  TNode<Object> next =
      LoadObjectField(request, AsyncGeneratorRequest::kNextOffset);

  StoreObjectField(generator, JSAsyncGeneratorObject::kQueueOffset, next);
  return request;
}
}  // namespace

// https://tc39.github.io/proposal-async-iteration/
// Section #sec-asyncgenerator-prototype-next
TF_BUILTIN(AsyncGeneratorPrototypeNext, AsyncGeneratorBuiltinsAssembler) {
  const int kValueArg = 0;

  TNode<IntPtrT> argc = ChangeInt32ToIntPtr(
      UncheckedCast<Int32T>(Parameter(Descriptor::kJSActualArgumentsCount)));
  CodeStubArguments args(this, argc);

  TNode<Object> generator = args.GetReceiver();
  TNode<Object> value = args.GetOptionalArgumentValue(kValueArg);
  TNode<Context> context = CAST(Parameter(Descriptor::kContext));

  AsyncGeneratorEnqueue(&args, context, generator, value,
                        JSAsyncGeneratorObject::kNext,
                        "[AsyncGenerator].prototype.next");
}

// https://tc39.github.io/proposal-async-iteration/
// Section #sec-asyncgenerator-prototype-return
TF_BUILTIN(AsyncGeneratorPrototypeReturn, AsyncGeneratorBuiltinsAssembler) {
  const int kValueArg = 0;

  TNode<IntPtrT> argc = ChangeInt32ToIntPtr(
      UncheckedCast<Int32T>(Parameter(Descriptor::kJSActualArgumentsCount)));
  CodeStubArguments args(this, argc);

  TNode<Object> generator = args.GetReceiver();
  TNode<Object> value = args.GetOptionalArgumentValue(kValueArg);
  TNode<Context> context = CAST(Parameter(Descriptor::kContext));

  AsyncGeneratorEnqueue(&args, context, generator, value,
                        JSAsyncGeneratorObject::kReturn,
                        "[AsyncGenerator].prototype.return");
}

// https://tc39.github.io/proposal-async-iteration/
// Section #sec-asyncgenerator-prototype-throw
TF_BUILTIN(AsyncGeneratorPrototypeThrow, AsyncGeneratorBuiltinsAssembler) {
  const int kValueArg = 0;

  TNode<IntPtrT> argc = ChangeInt32ToIntPtr(
      UncheckedCast<Int32T>(Parameter(Descriptor::kJSActualArgumentsCount)));
  CodeStubArguments args(this, argc);

  TNode<Object> generator = args.GetReceiver();
  TNode<Object> value = args.GetOptionalArgumentValue(kValueArg);
  TNode<Context> context = CAST(Parameter(Descriptor::kContext));

  AsyncGeneratorEnqueue(&args, context, generator, value,
                        JSAsyncGeneratorObject::kThrow,
                        "[AsyncGenerator].prototype.throw");
}

TF_BUILTIN(AsyncGeneratorAwaitResolveClosure, AsyncGeneratorBuiltinsAssembler) {
  TNode<Object> value = CAST(Parameter(Descriptor::kValue));
  TNode<Context> context = CAST(Parameter(Descriptor::kContext));
  AsyncGeneratorAwaitResumeClosure(context, value,
                                   JSAsyncGeneratorObject::kNext);
}

TF_BUILTIN(AsyncGeneratorAwaitRejectClosure, AsyncGeneratorBuiltinsAssembler) {
  TNode<Object> value = CAST(Parameter(Descriptor::kValue));
  TNode<Context> context = CAST(Parameter(Descriptor::kContext));
  AsyncGeneratorAwaitResumeClosure(context, value,
                                   JSAsyncGeneratorObject::kThrow);
}

TF_BUILTIN(AsyncGeneratorAwaitUncaught, AsyncGeneratorBuiltinsAssembler) {
  const bool kIsCatchable = false;
  AsyncGeneratorAwait<Descriptor>(kIsCatchable);
}

TF_BUILTIN(AsyncGeneratorAwaitCaught, AsyncGeneratorBuiltinsAssembler) {
  const bool kIsCatchable = true;
  AsyncGeneratorAwait<Descriptor>(kIsCatchable);
}

TF_BUILTIN(AsyncGeneratorResumeNext, AsyncGeneratorBuiltinsAssembler) {
  using Descriptor = AsyncGeneratorResumeNextDescriptor;
  const TNode<JSAsyncGeneratorObject> generator =
      CAST(Parameter(Descriptor::kGenerator));
  const TNode<Context> context = CAST(Parameter(Descriptor::kContext));

  // The penultimate step of proposal-async-iteration/#sec-asyncgeneratorresolve
  // and proposal-async-iteration/#sec-asyncgeneratorreject both recursively
  // invoke AsyncGeneratorResumeNext() again.
  //
  // This implementation does not implement this recursively, but instead
  // performs a loop in AsyncGeneratorResumeNext, which  continues as long as
  // there is an AsyncGeneratorRequest in the queue, and as long as the
  // generator is not suspended due to an AwaitExpression.
  TVARIABLE(Smi, var_state, LoadGeneratorState(generator));
  TVARIABLE(HeapObject, var_next,
            LoadFirstAsyncGeneratorRequestFromQueue(generator));
  Label start(this, {&var_state, &var_next});
  Goto(&start);
  BIND(&start);

  CSA_ASSERT(this, IsGeneratorNotExecuting(generator));

  // Stop resuming if suspended for Await.
  ReturnIf(IsGeneratorAwaiting(generator), UndefinedConstant());

  // Stop resuming if request queue is empty.
  ReturnIf(IsUndefined(var_next.value()), UndefinedConstant());

  const TNode<AsyncGeneratorRequest> next = CAST(var_next.value());
  const TNode<Smi> resume_type = LoadResumeTypeFromAsyncGeneratorRequest(next);

  Label if_abrupt(this), if_normal(this), resume_generator(this);
  Branch(IsAbruptResumeType(resume_type), &if_abrupt, &if_normal);
  BIND(&if_abrupt);
  {
    Label settle_promise(this), if_return(this), if_throw(this);
    GotoIfNot(IsGeneratorStateSuspendedAtStart(var_state.value()),
              &settle_promise);
    CloseGenerator(generator);
    var_state = SmiConstant(JSGeneratorObject::kGeneratorClosed);
    Goto(&settle_promise);

    BIND(&settle_promise);
    TNode<Object> next_value = LoadValueFromAsyncGeneratorRequest(next);
    Branch(SmiEqual(resume_type, SmiConstant(JSGeneratorObject::kReturn)),
           &if_return, &if_throw);

    BIND(&if_return);
    // For "return" completions, await the sent value. If the Await succeeds,
    // and the generator is not closed, resume the generator with a "return"
    // completion to allow `finally` blocks to be evaluated. Otherwise, perform
    // AsyncGeneratorResolve(awaitedValue, true). If the await fails and the
    // generator is not closed, resume the generator with a "throw" completion.
    // If the generator was closed, perform AsyncGeneratorReject(thrownValue).
    // In all cases, the last step is to call AsyncGeneratorResumeNext.
    TNode<Object> is_caught = CallRuntime(
        Runtime::kAsyncGeneratorHasCatchHandlerForPC, context, generator);
    TailCallBuiltin(Builtins::kAsyncGeneratorReturn, context, generator,
                    next_value, is_caught);

    BIND(&if_throw);
    GotoIfNot(IsGeneratorStateClosed(var_state.value()), &resume_generator);
    CallBuiltin(Builtins::kAsyncGeneratorReject, context, generator,
                next_value);
    var_next = LoadFirstAsyncGeneratorRequestFromQueue(generator);
    Goto(&start);
  }

  BIND(&if_normal);
  {
    GotoIfNot(IsGeneratorStateClosed(var_state.value()), &resume_generator);
    CallBuiltin(Builtins::kAsyncGeneratorResolve, context, generator,
                UndefinedConstant(), TrueConstant());
    var_state = LoadGeneratorState(generator);
    var_next = LoadFirstAsyncGeneratorRequestFromQueue(generator);
    Goto(&start);
  }

  BIND(&resume_generator);
  {
    // Remember the {resume_type} for the {generator}.
    StoreObjectFieldNoWriteBarrier(
        generator, JSGeneratorObject::kResumeModeOffset, resume_type);
    CallStub(CodeFactory::ResumeGenerator(isolate()), context,
             LoadValueFromAsyncGeneratorRequest(next), generator);
    var_state = LoadGeneratorState(generator);
    var_next = LoadFirstAsyncGeneratorRequestFromQueue(generator);
    Goto(&start);
  }
}

TF_BUILTIN(AsyncGeneratorResolve, AsyncGeneratorBuiltinsAssembler) {
  const TNode<JSAsyncGeneratorObject> generator =
      CAST(Parameter(Descriptor::kGenerator));
  const TNode<Object> value = CAST(Parameter(Descriptor::kValue));
  const TNode<Object> done = CAST(Parameter(Descriptor::kDone));
  const TNode<Context> context = CAST(Parameter(Descriptor::kContext));

  CSA_ASSERT(this, Word32BinaryNot(IsGeneratorAwaiting(generator)));

  // This operation should be called only when the `value` parameter has been
  // Await-ed. Typically, this means `value` is not a JSPromise value. However,
  // it may be a JSPromise value whose "then" method has been overridden to a
  // non-callable value. This can't be checked with assertions due to being
  // observable, but keep it in mind.

  const TNode<AsyncGeneratorRequest> next =
      TakeFirstAsyncGeneratorRequestFromQueue(generator);
  const TNode<JSPromise> promise = LoadPromiseFromAsyncGeneratorRequest(next);

  // Let iteratorResult be CreateIterResultObject(value, done).
  const TNode<HeapObject> iter_result = Allocate(JSIteratorResult::kSize);
  {
    TNode<Map> map = CAST(LoadContextElement(
        LoadNativeContext(context), Context::ITERATOR_RESULT_MAP_INDEX));
    StoreMapNoWriteBarrier(iter_result, map);
    StoreObjectFieldRoot(iter_result, JSIteratorResult::kPropertiesOrHashOffset,
                         RootIndex::kEmptyFixedArray);
    StoreObjectFieldRoot(iter_result, JSIteratorResult::kElementsOffset,
                         RootIndex::kEmptyFixedArray);
    StoreObjectFieldNoWriteBarrier(iter_result, JSIteratorResult::kValueOffset,
                                   value);
    StoreObjectFieldNoWriteBarrier(iter_result, JSIteratorResult::kDoneOffset,
                                   done);
  }

  // We know that {iter_result} itself doesn't have any "then" property (a
  // freshly allocated IterResultObject only has "value" and "done" properties)
  // and we also know that the [[Prototype]] of {iter_result} is the intrinsic
  // %ObjectPrototype%. So we can skip the [[Resolve]] logic here completely
  // and directly call into the FulfillPromise operation if we can prove
  // that the %ObjectPrototype% also doesn't have any "then" property. This
  // is guarded by the Promise#then() protector.
  // If the PromiseHooks are enabled, we cannot take the shortcut here, since
  // the "promiseResolve" hook would not be fired otherwise.
  Label if_fast(this), if_slow(this, Label::kDeferred), return_promise(this);
  GotoIfForceSlowPath(&if_slow);
  GotoIf(IsIsolatePromiseHookEnabledOrHasAsyncEventDelegate(), &if_slow);
  Branch(IsPromiseThenProtectorCellInvalid(), &if_slow, &if_fast);

  BIND(&if_fast);
  {
    // Skip the "then" on {iter_result} and directly fulfill the {promise}
    // with the {iter_result}.
    CallBuiltin(Builtins::kFulfillPromise, context, promise, iter_result);
    Goto(&return_promise);
  }

  BIND(&if_slow);
  {
    // Perform Call(promiseCapability.[[Resolve]], undefined, «iteratorResult»).
    CallBuiltin(Builtins::kResolvePromise, context, promise, iter_result);
    Goto(&return_promise);
  }

  // Per spec, AsyncGeneratorResolve() returns undefined. However, for the
  // benefit of %TraceExit(), return the Promise.
  BIND(&return_promise);
  Return(promise);
}

TF_BUILTIN(AsyncGeneratorReject, AsyncGeneratorBuiltinsAssembler) {
  using Descriptor = AsyncGeneratorRejectDescriptor;
  const TNode<JSAsyncGeneratorObject> generator =
      CAST(Parameter(Descriptor::kGenerator));
  const TNode<Object> value = CAST(Parameter(Descriptor::kValue));
  const TNode<Context> context = CAST(Parameter(Descriptor::kContext));

  TNode<AsyncGeneratorRequest> next =
      TakeFirstAsyncGeneratorRequestFromQueue(generator);
  TNode<JSPromise> promise = LoadPromiseFromAsyncGeneratorRequest(next);

  Return(CallBuiltin(Builtins::kRejectPromise, context, promise, value,
                     TrueConstant()));
}

TF_BUILTIN(AsyncGeneratorYield, AsyncGeneratorBuiltinsAssembler) {
  const TNode<JSGeneratorObject> generator =
      CAST(Parameter(Descriptor::kGenerator));
  const TNode<Object> value = CAST(Parameter(Descriptor::kValue));
  const TNode<Oddball> is_caught = CAST(Parameter(Descriptor::kIsCaught));
  const TNode<Context> context = CAST(Parameter(Descriptor::kContext));

  const TNode<AsyncGeneratorRequest> request =
      CAST(LoadFirstAsyncGeneratorRequestFromQueue(generator));
  const TNode<JSPromise> outer_promise =
      LoadPromiseFromAsyncGeneratorRequest(request);

  SetGeneratorAwaiting(generator);
  Await(context, generator, value, outer_promise,
        AsyncGeneratorYieldResolveSharedFunConstant(),
        AsyncGeneratorAwaitRejectSharedFunConstant(), is_caught);
  Return(UndefinedConstant());
}

TF_BUILTIN(AsyncGeneratorYieldResolveClosure, AsyncGeneratorBuiltinsAssembler) {
  const TNode<Context> context = CAST(Parameter(Descriptor::kContext));
  const TNode<Object> value = CAST(Parameter(Descriptor::kValue));
  const TNode<JSAsyncGeneratorObject> generator =
      CAST(LoadContextElement(context, Context::EXTENSION_INDEX));

  SetGeneratorNotAwaiting(generator);

  // Per proposal-async-iteration/#sec-asyncgeneratoryield step 9
  // Return ! AsyncGeneratorResolve(_F_.[[Generator]], _value_, *false*).
  CallBuiltin(Builtins::kAsyncGeneratorResolve, context, generator, value,
              FalseConstant());

  TailCallBuiltin(Builtins::kAsyncGeneratorResumeNext, context, generator);
}

TF_BUILTIN(AsyncGeneratorReturn, AsyncGeneratorBuiltinsAssembler) {
  // AsyncGeneratorReturn is called when resuming requests with "return" resume
  // modes. It is similar to AsyncGeneratorAwait(), but selects different
  // resolve/reject closures depending on whether or not the generator is marked
  // as closed.
  //
  // In particular, non-closed generators will resume the generator with either
  // "return" or "throw" resume modes, allowing finally blocks or catch blocks
  // to be evaluated, as if the `await` were performed within the body of the
  // generator. (per proposal-async-iteration/#sec-asyncgeneratoryield step 8.b)
  //
  // Closed generators do not resume the generator in the resolve/reject
  // closures, but instead simply perform AsyncGeneratorResolve or
  // AsyncGeneratorReject with the awaited value
  // (per proposal-async-iteration/#sec-asyncgeneratorresumenext step 10.b.i)
  //
  // In all cases, the final step is to jump back to AsyncGeneratorResumeNext.
  const TNode<JSGeneratorObject> generator =
      CAST(Parameter(Descriptor::kGenerator));
  const TNode<Object> value = CAST(Parameter(Descriptor::kValue));
  const TNode<Oddball> is_caught = CAST(Parameter(Descriptor::kIsCaught));
  const TNode<AsyncGeneratorRequest> req =
      CAST(LoadFirstAsyncGeneratorRequestFromQueue(generator));

  Label perform_await(this);
  TVARIABLE(SharedFunctionInfo, var_on_resolve,
            AsyncGeneratorReturnClosedResolveSharedFunConstant());

  TVARIABLE(SharedFunctionInfo, var_on_reject,
            AsyncGeneratorReturnClosedRejectSharedFunConstant());

  const TNode<Smi> state = LoadGeneratorState(generator);
  GotoIf(IsGeneratorStateClosed(state), &perform_await);
  var_on_resolve = AsyncGeneratorReturnResolveSharedFunConstant();
  var_on_reject = AsyncGeneratorAwaitRejectSharedFunConstant();

  Goto(&perform_await);

  BIND(&perform_await);

  SetGeneratorAwaiting(generator);
  TNode<Context> context = CAST(Parameter(Descriptor::kContext));
  const TNode<JSPromise> outer_promise =
      LoadPromiseFromAsyncGeneratorRequest(req);
  Await(context, generator, value, outer_promise, var_on_resolve.value(),
        var_on_reject.value(), is_caught);

  Return(UndefinedConstant());
}

// On-resolve closure for Await in AsyncGeneratorReturn
// Resume the generator with "return" resume_mode, and finally perform
// AsyncGeneratorResumeNext. Per
// proposal-async-iteration/#sec-asyncgeneratoryield step 8.e
TF_BUILTIN(AsyncGeneratorReturnResolveClosure,
           AsyncGeneratorBuiltinsAssembler) {
  const TNode<Context> context = CAST(Parameter(Descriptor::kContext));
  const TNode<Object> value = CAST(Parameter(Descriptor::kValue));
  AsyncGeneratorAwaitResumeClosure(context, value, JSGeneratorObject::kReturn);
}

// On-resolve closure for Await in AsyncGeneratorReturn
// Perform AsyncGeneratorResolve({awaited_value}, true) and finally perform
// AsyncGeneratorResumeNext.
TF_BUILTIN(AsyncGeneratorReturnClosedResolveClosure,
           AsyncGeneratorBuiltinsAssembler) {
  const TNode<Context> context = CAST(Parameter(Descriptor::kContext));
  const TNode<Object> value = CAST(Parameter(Descriptor::kValue));
  const TNode<JSAsyncGeneratorObject> generator =
      CAST(LoadContextElement(context, Context::EXTENSION_INDEX));

  SetGeneratorNotAwaiting(generator);

  // https://tc39.github.io/proposal-async-iteration/
  //    #async-generator-resume-next-return-processor-fulfilled step 2:
  //  Return ! AsyncGeneratorResolve(_F_.[[Generator]], _value_, *true*).
  CallBuiltin(Builtins::kAsyncGeneratorResolve, context, generator, value,
              TrueConstant());

  TailCallBuiltin(Builtins::kAsyncGeneratorResumeNext, context, generator);
}

TF_BUILTIN(AsyncGeneratorReturnClosedRejectClosure,
           AsyncGeneratorBuiltinsAssembler) {
  const TNode<Context> context = CAST(Parameter(Descriptor::kContext));
  const TNode<Object> value = CAST(Parameter(Descriptor::kValue));
  const TNode<JSAsyncGeneratorObject> generator =
      CAST(LoadContextElement(context, Context::EXTENSION_INDEX));

  SetGeneratorNotAwaiting(generator);

  // https://tc39.github.io/proposal-async-iteration/
  //    #async-generator-resume-next-return-processor-rejected step 2:
  // Return ! AsyncGeneratorReject(_F_.[[Generator]], _reason_).
  CallBuiltin(Builtins::kAsyncGeneratorReject, context, generator, value);

  TailCallBuiltin(Builtins::kAsyncGeneratorResumeNext, context, generator);
}

}  // namespace internal
}  // namespace v8
