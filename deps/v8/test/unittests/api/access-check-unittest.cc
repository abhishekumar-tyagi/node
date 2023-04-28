// Copyright 2017 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "include/v8-context.h"
#include "include/v8-function.h"
#include "include/v8-isolate.h"
#include "include/v8-local-handle.h"
#include "include/v8-primitive.h"
#include "include/v8-script.h"
#include "include/v8-template.h"
#include "src/debug/debug.h"
#include "test/unittests/test-utils.h"
#include "testing/gmock-support.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace v8 {

using testing::IsInt32;
using testing::IsString;

int32_t g_cross_context_int = 0;

bool g_expect_interceptor_call = false;

class AccessCheckTest : public TestWithIsolate {
 public:
  void CheckCanRunScriptInContext(Local<Context> context) {
    HandleScope handle_scope(isolate());
    Context::Scope context_scope(context);

    g_expect_interceptor_call = false;
    g_cross_context_int = 0;

    // Running script in this context should work.
    RunJS("this.foo = 42; this[23] = true;");
    EXPECT_THAT(RunJS("this.all_can_read"), IsInt32(42));
    RunJS("this.cross_context_int = 23");
    CHECK_EQ(g_cross_context_int, 23);
    EXPECT_THAT(RunJS("this.cross_context_int"), IsInt32(23));
  }

  void CheckCrossContextAccess(Local<Context> accessing_context,
                               Local<Object> accessed_object) {
    HandleScope handle_scope(isolate());
    accessing_context->Global()
        ->Set(accessing_context, NewString("other"), accessed_object)
        .FromJust();
    Context::Scope context_scope(accessing_context);

    g_expect_interceptor_call = true;
    g_cross_context_int = 23;

    {
      TryCatch try_catch(isolate());
      CHECK(TryRunJS("this.other.foo").IsEmpty());
    }
    {
      TryCatch try_catch(isolate());
      CHECK(TryRunJS("this.other[23]").IsEmpty());
    }

    // AllCanRead properties are also inaccessible.
    {
      TryCatch try_catch(isolate());
      CHECK(TryRunJS("this.other.all_can_read").IsEmpty());
    }

    // Intercepted properties are accessible, however.
    EXPECT_THAT(RunJS("this.other.cross_context_int"), IsInt32(23));
    RunJS("this.other.cross_context_int = 42");
    EXPECT_THAT(RunJS("this.other[7]"), IsInt32(42));
    EXPECT_THAT(RunJS("JSON.stringify(Object.getOwnPropertyNames(this.other))"),
                IsString("[\"7\",\"cross_context_int\"]"));
  }

  void CheckCrossContextAccessWithException(Local<Context> accessing_context,
                                            Local<Object> accessed_object) {
    HandleScope handle_scope(isolate());
    accessing_context->Global()
        ->Set(accessing_context, NewString("other"), accessed_object)
        .FromJust();
    Context::Scope context_scope(accessing_context);

    {
      TryCatch try_catch(isolate());
      TryRunJS("this.other.should_throw");
      CHECK(try_catch.HasCaught());
      CHECK(try_catch.Exception()->IsString());
      CHECK(NewString("exception")
                ->Equals(accessing_context, try_catch.Exception())
                .FromJust());
    }

    {
      TryCatch try_catch(isolate());
      TryRunJS("this.other.should_throw = 8");
      CHECK(try_catch.HasCaught());
      CHECK(try_catch.Exception()->IsString());
      CHECK(NewString("exception")
                ->Equals(accessing_context, try_catch.Exception())
                .FromJust());
    }

    {
      TryCatch try_catch(isolate());
      TryRunJS("this.other[42]");
      CHECK(try_catch.HasCaught());
      CHECK(try_catch.Exception()->IsString());
      CHECK(NewString("exception")
                ->Equals(accessing_context, try_catch.Exception())
                .FromJust());
    }

    {
      TryCatch try_catch(isolate());
      TryRunJS("this.other[42] = 8");
      CHECK(try_catch.HasCaught());
      CHECK(try_catch.Exception()->IsString());
      CHECK(NewString("exception")
                ->Equals(accessing_context, try_catch.Exception())
                .FromJust());
    }
  }
};

namespace {

bool AccessCheck(Local<Context> accessing_context,
                 Local<Object> accessed_object, Local<Value> data) {
  return false;
}

MaybeLocal<Value> CompileRun(Isolate* isolate, const char* source) {
  Local<String> source_string =
      String::NewFromUtf8(isolate, source).ToLocalChecked();
  Local<Context> context = isolate->GetCurrentContext();
  Local<Script> script =
      Script::Compile(context, source_string).ToLocalChecked();
  return script->Run(context);
}

}  // namespace

TEST_F(AccessCheckTest, GetOwnPropertyDescriptor) {
  isolate()->SetFailedAccessCheckCallbackFunction(
      [](v8::Local<v8::Object> host, v8::AccessType type,
         v8::Local<v8::Value> data) {});
  Local<ObjectTemplate> global_template = ObjectTemplate::New(isolate());
  global_template->SetAccessCheckCallback(AccessCheck);

  Local<FunctionTemplate> getter_template = FunctionTemplate::New(
      isolate(), [](const FunctionCallbackInfo<Value>& info) { FAIL(); });
  getter_template->SetAcceptAnyReceiver(false);
  Local<FunctionTemplate> setter_template = FunctionTemplate::New(
      isolate(), [](const FunctionCallbackInfo<v8::Value>& info) { FAIL(); });
  setter_template->SetAcceptAnyReceiver(false);
  global_template->SetAccessorProperty(NewString("property"), getter_template,
                                       setter_template);

  Local<Context> target_context =
      Context::New(isolate(), nullptr, global_template);
  Local<Context> accessing_context =
      Context::New(isolate(), nullptr, global_template);

  accessing_context->Global()
      ->Set(accessing_context, NewString("other"), target_context->Global())
      .FromJust();

  Context::Scope context_scope(accessing_context);
  Local<Value> result =
      CompileRun(isolate(),
                 "Object.getOwnPropertyDescriptor(this, 'property')"
                 "    .get.call(other);")
          .ToLocalChecked();
  EXPECT_TRUE(result->IsUndefined());
  CompileRun(isolate(),
             "Object.getOwnPropertyDescriptor(this, 'property')"
             "    .set.call(other, 42);");
}

class AccessRegressionTest : public AccessCheckTest {
 protected:
  i::Handle<i::JSFunction> RetrieveFunctionFrom(Local<Context> context,
                                                const char* script) {
    Context::Scope context_scope(context);
    Local<Value> getter = CompileRun(isolate(), script).ToLocalChecked();
    EXPECT_TRUE(getter->IsFunction());

    i::Handle<i::JSReceiver> r =
        Utils::OpenHandle(*Local<Function>::Cast(getter));
    EXPECT_TRUE(r->IsJSFunction());
    return i::Handle<i::JSFunction>::cast(r);
  }
};

TEST_F(AccessRegressionTest,
       InstantiatedLazyAccessorPairsHaveCorrectNativeContext) {
  // The setup creates two contexts and sets an object created
  // in context 1 on the global of context 2.
  // The object has an accessor pair {property}. Accessing the
  // property descriptor of {property} causes instantiation of the
  // accessor pair. The test checks that the access pair has the
  // correct native context.
  Local<FunctionTemplate> getter_template = FunctionTemplate::New(
      isolate(), [](const FunctionCallbackInfo<Value>&) { FAIL(); });
  Local<FunctionTemplate> setter_template = FunctionTemplate::New(
      isolate(), [](const FunctionCallbackInfo<v8::Value>&) { FAIL(); });

  Local<ObjectTemplate> object_template = ObjectTemplate::New(isolate());
  object_template->SetAccessorProperty(NewString("property"), getter_template,
                                       setter_template);

  Local<Context> context1 = Context::New(isolate(), nullptr);
  Local<Context> context2 = Context::New(isolate(), nullptr);

  Local<Object> object =
      object_template->NewInstance(context1).ToLocalChecked();
  context2->Global()
      ->Set(context2, NewString("object_from_context1"), object)
      .Check();

  i::Handle<i::JSFunction> getter = RetrieveFunctionFrom(
      context2,
      "Object.getOwnPropertyDescriptor(object_from_context1, 'property').get");

  ASSERT_EQ(getter->native_context(), *Utils::OpenHandle(*context1));
}

// Regression test for https://crbug.com/986063.
TEST_F(AccessRegressionTest,
       InstantiatedLazyAccessorPairsHaveCorrectNativeContextDebug) {
  // The setup creates two contexts and installs an object "object"
  // on the global this for each context.
  // The object consists of:
  //    - an accessor pair "property".
  //    - a normal function "breakfn".
  //
  // The test sets a break point on {object.breakfn} in the first context.
  // This forces instantation of the JSFunction for the {object.property}
  // accessor pair. The test verifies afterwards that the respective
  // JSFunction of the getter have the correct native context.

  Local<FunctionTemplate> getter_template = FunctionTemplate::New(
      isolate(), [](const FunctionCallbackInfo<Value>&) { FAIL(); });
  Local<FunctionTemplate> setter_template = FunctionTemplate::New(
      isolate(), [](const FunctionCallbackInfo<v8::Value>&) { FAIL(); });
  Local<FunctionTemplate> break_template = FunctionTemplate::New(
      isolate(), [](const FunctionCallbackInfo<v8::Value>&) { FAIL(); });

  Local<Context> context1 = Context::New(isolate(), nullptr);
  Local<Context> context2 = Context::New(isolate(), nullptr);

  Local<ObjectTemplate> object_template = ObjectTemplate::New(isolate());
  object_template->Set(isolate(), "breakfn", break_template);
  object_template->SetAccessorProperty(NewString("property"), getter_template,
                                       setter_template);

  Local<Object> object1 =
      object_template->NewInstance(context1).ToLocalChecked();
  EXPECT_TRUE(
      context1->Global()->Set(context1, NewString("object"), object1).IsJust());

  Local<Object> object2 =
      object_template->NewInstance(context2).ToLocalChecked();
  EXPECT_TRUE(
      context2->Global()->Set(context2, NewString("object"), object2).IsJust());

  // Force instantiation of the JSFunction for the getter and setter
  // of {object.property} by setting a break point on {object.breakfn}
  {
    Context::Scope context_scope(context1);
    i::Isolate* iso = reinterpret_cast<i::Isolate*>(isolate());
    i::Handle<i::JSFunction> break_fn =
        RetrieveFunctionFrom(context1, "object.breakfn");

    int id;
    iso->debug()->SetBreakpointForFunction(i::handle(break_fn->shared(), iso),
                                           iso->factory()->empty_string(), &id);
  }

  i::Handle<i::JSFunction> getter_c1 = RetrieveFunctionFrom(
      context1, "Object.getOwnPropertyDescriptor(object, 'property').get");
  i::Handle<i::JSFunction> getter_c2 = RetrieveFunctionFrom(
      context2, "Object.getOwnPropertyDescriptor(object, 'property').get");

  ASSERT_EQ(getter_c1->native_context(), *Utils::OpenHandle(*context1));
  ASSERT_EQ(getter_c2->native_context(), *Utils::OpenHandle(*context2));
}

void NamedGetter(Local<Name> property,
                 const PropertyCallbackInfo<Value>& info) {
  CHECK(g_expect_interceptor_call);
  Isolate* isolate = info.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  if (property
          ->Equals(context, String::NewFromUtf8(isolate, "cross_context_int")
                                .ToLocalChecked())
          .FromJust())
    info.GetReturnValue().Set(g_cross_context_int);
}

void NamedSetter(Local<Name> property, Local<Value> value,
                 const PropertyCallbackInfo<Value>& info) {
  CHECK(g_expect_interceptor_call);
  Isolate* isolate = info.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  if (!property
           ->Equals(context, String::NewFromUtf8(isolate, "cross_context_int")
                                 .ToLocalChecked())
           .FromJust())
    return;
  if (value->IsInt32()) {
    g_cross_context_int = value->ToInt32(context).ToLocalChecked()->Value();
  }
  info.GetReturnValue().Set(value);
}

void NamedQuery(Local<Name> property,
                const PropertyCallbackInfo<Integer>& info) {
  CHECK(g_expect_interceptor_call);
  Isolate* isolate = info.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  if (!property
           ->Equals(context, String::NewFromUtf8(isolate, "cross_context_int")
                                 .ToLocalChecked())
           .FromJust())
    return;
  info.GetReturnValue().Set(DontDelete);
}

void NamedDeleter(Local<Name> property,
                  const PropertyCallbackInfo<Boolean>& info) {
  CHECK(g_expect_interceptor_call);
  Isolate* isolate = info.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  if (!property
           ->Equals(context, String::NewFromUtf8(isolate, "cross_context_int")
                                 .ToLocalChecked())
           .FromJust())
    return;
  info.GetReturnValue().Set(false);
}

void NamedEnumerator(const PropertyCallbackInfo<Array>& info) {
  CHECK(g_expect_interceptor_call);
  Isolate* isolate = info.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  Local<Array> names = Array::New(isolate, 1);
  names
      ->Set(context, 0,
            String::NewFromUtf8(isolate, "cross_context_int").ToLocalChecked())
      .FromJust();
  info.GetReturnValue().Set(names);
}

void IndexedGetter(uint32_t index, const PropertyCallbackInfo<Value>& info) {
  CHECK(g_expect_interceptor_call);
  if (index == 7) info.GetReturnValue().Set(g_cross_context_int);
}

void IndexedSetter(uint32_t index, Local<Value> value,
                   const PropertyCallbackInfo<Value>& info) {
  CHECK(g_expect_interceptor_call);
  Isolate* isolate = info.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  if (index != 7) return;
  if (value->IsInt32()) {
    g_cross_context_int = value->ToInt32(context).ToLocalChecked()->Value();
  }
  info.GetReturnValue().Set(value);
}

void IndexedQuery(uint32_t index, const PropertyCallbackInfo<Integer>& info) {
  CHECK(g_expect_interceptor_call);
  if (index == 7) info.GetReturnValue().Set(DontDelete);
}

void IndexedDeleter(uint32_t index, const PropertyCallbackInfo<Boolean>& info) {
  CHECK(g_expect_interceptor_call);
  if (index == 7) info.GetReturnValue().Set(false);
}

void IndexedEnumerator(const PropertyCallbackInfo<Array>& info) {
  CHECK(g_expect_interceptor_call);
  Isolate* isolate = info.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  Local<Array> names = Array::New(isolate, 1);
  names->Set(context, 0, String::NewFromUtf8(isolate, "7").ToLocalChecked())
      .FromJust();
  info.GetReturnValue().Set(names);
}

void MethodGetter(Local<Name> property,
                  const PropertyCallbackInfo<Value>& info) {
  Isolate* isolate = info.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();

  Local<External> data = info.Data().As<External>();
  Local<FunctionTemplate>& function_template =
      *reinterpret_cast<Local<FunctionTemplate>*>(data->Value());

  info.GetReturnValue().Set(
      function_template->GetFunction(context).ToLocalChecked());
}

void MethodCallback(const FunctionCallbackInfo<Value>& info) {
  info.GetReturnValue().Set(8);
}

void NamedGetterThrowsException(Local<Name> property,
                                const PropertyCallbackInfo<Value>& info) {
  info.GetIsolate()->ThrowException(
      String::NewFromUtf8(info.GetIsolate(), "exception").ToLocalChecked());
}

void NamedSetterThrowsException(Local<Name> property, Local<Value> value,
                                const PropertyCallbackInfo<Value>& info) {
  info.GetIsolate()->ThrowException(
      String::NewFromUtf8(info.GetIsolate(), "exception").ToLocalChecked());
}

void IndexedGetterThrowsException(uint32_t index,
                                  const PropertyCallbackInfo<Value>& info) {
  info.GetIsolate()->ThrowException(
      String::NewFromUtf8(info.GetIsolate(), "exception").ToLocalChecked());
}

void IndexedSetterThrowsException(uint32_t index, Local<Value> value,
                                  const PropertyCallbackInfo<Value>& info) {
  info.GetIsolate()->ThrowException(
      String::NewFromUtf8(info.GetIsolate(), "exception").ToLocalChecked());
}

void GetCrossContextInt(Local<String> property,
                        const PropertyCallbackInfo<Value>& info) {
  CHECK(!g_expect_interceptor_call);
  info.GetReturnValue().Set(g_cross_context_int);
}

void SetCrossContextInt(Local<String> property, Local<Value> value,
                        const PropertyCallbackInfo<void>& info) {
  CHECK(!g_expect_interceptor_call);
  Isolate* isolate = info.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  if (value->IsInt32()) {
    g_cross_context_int = value->ToInt32(context).ToLocalChecked()->Value();
  }
}

void Return42(Local<String> property, const PropertyCallbackInfo<Value>& info) {
  info.GetReturnValue().Set(42);
}

void Ctor(const FunctionCallbackInfo<Value>& info) {
  CHECK(info.IsConstructCall());
}

TEST_F(AccessCheckTest, AccessCheckWithInterceptor) {
  Local<ObjectTemplate> global_template = ObjectTemplate::New(isolate());
  global_template->SetAccessCheckCallbackAndHandler(
      AccessCheck,
      NamedPropertyHandlerConfiguration(NamedGetter, NamedSetter, NamedQuery,
                                        NamedDeleter, NamedEnumerator),
      IndexedPropertyHandlerConfiguration(IndexedGetter, IndexedSetter,
                                          IndexedQuery, IndexedDeleter,
                                          IndexedEnumerator));
  global_template->SetNativeDataProperty(
      NewString("cross_context_int"), GetCrossContextInt, SetCrossContextInt);
  global_template->SetNativeDataProperty(NewString("all_can_read"), Return42,
                                         nullptr, Local<Value>(), None,
                                         ALL_CAN_READ);

  Local<Context> context0 = Context::New(isolate(), nullptr, global_template);
  CheckCanRunScriptInContext(context0);

  // Create another context.
  Local<Context> context1 = Context::New(isolate(), nullptr, global_template);
  CheckCrossContextAccess(context1, context0->Global());
}

TEST_F(AccessCheckTest, CallFunctionWithRemoteContextReceiver) {
  HandleScope scope(isolate());
  Local<FunctionTemplate> global_template = FunctionTemplate::New(isolate());

  Local<Signature> signature = Signature::New(isolate(), global_template);
  Local<FunctionTemplate> function_template = FunctionTemplate::New(
      isolate(), MethodCallback, External::New(isolate(), &function_template),
      signature);

  global_template->InstanceTemplate()->SetAccessCheckCallbackAndHandler(
      AccessCheck,
      NamedPropertyHandlerConfiguration(
          MethodGetter, nullptr, nullptr, nullptr, nullptr,
          External::New(isolate(), &function_template)),
      IndexedPropertyHandlerConfiguration());

  Local<Object> accessed_object =
      Context::NewRemoteContext(isolate(), global_template->InstanceTemplate())
          .ToLocalChecked();
  Local<Context> accessing_context =
      Context::New(isolate(), nullptr, global_template->InstanceTemplate());

  HandleScope handle_scope(isolate());
  accessing_context->Global()
      ->Set(accessing_context, NewString("other"), accessed_object)
      .FromJust();
  Context::Scope context_scope(accessing_context);

  {
    TryCatch try_catch(isolate());
    EXPECT_THAT(RunJS("this.other.method()"), IsInt32(8));
    CHECK(!try_catch.HasCaught());
  }
}

TEST_F(AccessCheckTest, AccessCheckWithExceptionThrowingInterceptor) {
  isolate()->SetFailedAccessCheckCallbackFunction(
      [](Local<Object> target, AccessType type, Local<Value> data) {
        UNREACHABLE();  // This should never be called.
      });

  Local<ObjectTemplate> global_template = ObjectTemplate::New(isolate());
  global_template->SetAccessCheckCallbackAndHandler(
      AccessCheck,
      NamedPropertyHandlerConfiguration(NamedGetterThrowsException,
                                        NamedSetterThrowsException),
      IndexedPropertyHandlerConfiguration(IndexedGetterThrowsException,
                                          IndexedSetterThrowsException));

  // Create two contexts.
  Local<Context> context0 = Context::New(isolate(), nullptr, global_template);
  Local<Context> context1 = Context::New(isolate(), nullptr, global_template);

  CheckCrossContextAccessWithException(context1, context0->Global());
}

TEST_F(AccessCheckTest, NewRemoteContext) {
  Local<ObjectTemplate> global_template = ObjectTemplate::New(isolate());
  global_template->SetAccessCheckCallbackAndHandler(
      AccessCheck,
      NamedPropertyHandlerConfiguration(NamedGetter, NamedSetter, NamedQuery,
                                        NamedDeleter, NamedEnumerator),
      IndexedPropertyHandlerConfiguration(IndexedGetter, IndexedSetter,
                                          IndexedQuery, IndexedDeleter,
                                          IndexedEnumerator));
  global_template->SetNativeDataProperty(
      NewString("cross_context_int"), GetCrossContextInt, SetCrossContextInt);
  global_template->SetNativeDataProperty(NewString("all_can_read"), Return42,
                                         nullptr, Local<Value>(), None,
                                         ALL_CAN_READ);

  Local<Object> global0 =
      Context::NewRemoteContext(isolate(), global_template).ToLocalChecked();

  // Create a real context.
  {
    HandleScope other_scope(isolate());
    Local<Context> context1 = Context::New(isolate(), nullptr, global_template);

    CheckCrossContextAccess(context1, global0);
  }

  // Create a context using the detached global.
  {
    HandleScope other_scope(isolate());
    Local<Context> context2 =
        Context::New(isolate(), nullptr, global_template, global0);

    CheckCanRunScriptInContext(context2);
  }

  // Turn a regular context into a remote context.
  {
    HandleScope other_scope(isolate());
    Local<Context> context3 = Context::New(isolate(), nullptr, global_template);

    CheckCanRunScriptInContext(context3);

    // Turn the global object into a remote context, and try to access it.
    Local<Object> context3_global = context3->Global();
    context3->DetachGlobal();
    Local<Object> global3 =
        Context::NewRemoteContext(isolate(), global_template, context3_global)
            .ToLocalChecked();
    Local<Context> context4 = Context::New(isolate(), nullptr, global_template);

    CheckCrossContextAccess(context4, global3);

    // Turn it back into a regular context.
    Local<Context> context5 =
        Context::New(isolate(), nullptr, global_template, global3);

    CheckCanRunScriptInContext(context5);
  }
}

TEST_F(AccessCheckTest, NewRemoteInstance) {
  Local<FunctionTemplate> tmpl = FunctionTemplate::New(isolate(), Ctor);
  Local<ObjectTemplate> instance = tmpl->InstanceTemplate();
  instance->SetAccessCheckCallbackAndHandler(
      AccessCheck,
      NamedPropertyHandlerConfiguration(NamedGetter, NamedSetter, NamedQuery,
                                        NamedDeleter, NamedEnumerator),
      IndexedPropertyHandlerConfiguration(IndexedGetter, IndexedSetter,
                                          IndexedQuery, IndexedDeleter,
                                          IndexedEnumerator));
  tmpl->SetNativeDataProperty(NewString("all_can_read"), Return42, nullptr,
                              Local<Value>(), None, ALL_CAN_READ);

  Local<Object> obj = tmpl->NewRemoteInstance().ToLocalChecked();

  Local<Context> context = Context::New(isolate());
  CheckCrossContextAccess(context, obj);
}

}  // namespace v8
