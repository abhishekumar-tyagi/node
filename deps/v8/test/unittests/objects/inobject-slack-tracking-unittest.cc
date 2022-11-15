// Copyright 2015 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdlib.h>

#include <sstream>
#include <utility>

#include "src/api/api-inl.h"
#include "src/init/v8.h"
#include "src/objects/heap-number-inl.h"
#include "src/objects/objects-inl.h"
#include "test/unittests/test-utils.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace v8 {
namespace internal {
namespace test_inobject_slack_tracking {

static const int kMaxInobjectProperties = JSObject::kMaxInObjectProperties;

class InObjectSlackTrackingTest : public TestWithContext {
 public:
  template <typename T = Object>
  Handle<T> GetLexical(const char* name) {
    Factory* factory = i_isolate()->factory();

    Handle<String> str_name = factory->InternalizeUtf8String(name);
    Handle<ScriptContextTable> script_contexts(
        i_isolate()->native_context()->script_context_table(), i_isolate());

    VariableLookupResult lookup_result;
    if (script_contexts->Lookup(str_name, &lookup_result)) {
      Handle<Context> script_context = ScriptContextTable::GetContext(
          i_isolate(), script_contexts, lookup_result.context_index);

      Handle<Object> result(script_context->get(lookup_result.slot_index),
                            i_isolate());
      return Handle<T>::cast(result);
    }
    return Handle<T>();
  }

  template <typename T = Object>
  Handle<T> GetLexical(const std::string& name) {
    return GetLexical<T>(name.c_str());
  }

  bool IsObjectShrinkable(JSObject obj) {
    Handle<Map> filler_map = i_isolate()->factory()->one_pointer_filler_map();

    int inobject_properties = obj.map().GetInObjectProperties();
    int unused = obj.map().UnusedPropertyFields();
    if (unused == 0) return false;

    Address packed_filler = MapWord::FromMap(*filler_map).ptr();
    for (int i = inobject_properties - unused; i < inobject_properties; i++) {
      if (packed_filler != GetFieldValue(obj, i).ptr()) {
        return false;
      }
    }
    return true;
  }

  template <typename T>
  Handle<T> GetGlobal(const char* name) {
    Handle<String> str_name =
        i_isolate()->factory()->InternalizeUtf8String(name);

    Handle<Object> value =
        Object::GetProperty(i_isolate(), i_isolate()->global_object(), str_name)
            .ToHandleChecked();
    return Handle<T>::cast(value);
  }

  static Object GetFieldValue(JSObject obj, int property_index) {
    FieldIndex index = FieldIndex::ForPropertyIndex(obj.map(), property_index);
    return obj.RawFastPropertyAt(index);
  }

  static double GetDoubleFieldValue(JSObject obj, FieldIndex field_index) {
    Object value = obj.RawFastPropertyAt(field_index);
    if (value.IsHeapNumber()) {
      return HeapNumber::cast(value).value();
    } else {
      return value.Number();
    }
  }

  static double GetDoubleFieldValue(JSObject obj, int property_index) {
    FieldIndex index = FieldIndex::ForPropertyIndex(obj.map(), property_index);
    return GetDoubleFieldValue(obj, index);
  }

  Local<Value> CompileRun(const char* source) {
    Local<v8::String> source_string =
        v8::String::NewFromUtf8(isolate(), source).ToLocalChecked();
    Local<v8::Script> script =
        v8::Script::Compile(context(), source_string).ToLocalChecked();
    return script->Run(context()).ToLocalChecked();
  }

  v8::Local<v8::Script> Compile(const char* source) {
    v8::Local<v8::Script> result;
    CHECK(v8::Script::Compile(
              context(),
              v8::String::NewFromUtf8(isolate(), source).ToLocalChecked())
              .ToLocal(&result));
    return result;
  }

  template <typename T>
  static Handle<T> OpenHandle(v8::Local<v8::Value> value) {
    Handle<Object> obj = v8::Utils::OpenHandle(*value);
    return Handle<T>::cast(obj);
  }

  inline v8::Local<v8::Value> Run(v8::Local<v8::Script> script) {
    v8::Local<v8::Value> result;
    if (script->Run(context()).ToLocal(&result)) {
      return result;
    }
    return v8::Local<v8::Value>();
  }

  template <typename T>
  inline Handle<T> RunI(v8::Local<v8::Script> script) {
    return OpenHandle<T>(Run(script));
  }

  template <typename T>
  inline Handle<T> CompileRunI(const char* script) {
    return OpenHandle<T>(CompileRun(script));
  }

  static std::string GetClassName(int class_index) {
    std::ostringstream os;
    os << "A" << class_index;
    return os.str();
  }

  v8::Local<v8::Script> GetNewObjectScript(const std::string& class_name) {
    std::ostringstream os;
    os << "new " << class_name << "();";
    return Compile(os.str().c_str());
  }

  // Test that in-object slack tracking works as expected for first |n| classes
  // in the hierarchy.
  // This test works only for if the total property count is less than maximum
  // in-object properties count.
  void TestClassHierarchy(const std::vector<int>& hierarchy_desc, int n) {
    int fields_count = 0;
    for (int cur_class = 0; cur_class < n; cur_class++) {
      std::string class_name = GetClassName(cur_class);
      int fields_count_at_current_level = hierarchy_desc[cur_class];
      fields_count += fields_count_at_current_level;

      // This test is not suitable for in-object properties count overflow case.
      CHECK_LT(fields_count, kMaxInobjectProperties);

      // Create |class_name| objects and check slack tracking.
      v8::Local<v8::Script> new_script = GetNewObjectScript(class_name);

      Handle<JSFunction> func = GetLexical<JSFunction>(class_name);

      Handle<JSObject> obj = RunI<JSObject>(new_script);

      CHECK(func->has_initial_map());
      Handle<Map> initial_map(func->initial_map(), func->GetIsolate());

      // If the object is slow-mode already, bail out.
      if (obj->map().is_dictionary_map()) continue;

      // There must be at least some slack.
      CHECK_LT(fields_count, obj->map().GetInObjectProperties());

      // One instance was created.
      CHECK_EQ(Map::kSlackTrackingCounterStart - 1,
               initial_map->construction_counter());
      CHECK(initial_map->IsInobjectSlackTrackingInProgress());

      // Create several instances to complete the tracking.
      for (int i = 1; i < Map::kGenerousAllocationCount; i++) {
        CHECK(initial_map->IsInobjectSlackTrackingInProgress());
        Handle<JSObject> tmp = RunI<JSObject>(new_script);
        CHECK_EQ(initial_map->IsInobjectSlackTrackingInProgress(),
                 IsObjectShrinkable(*tmp));
        if (!initial_map->IsInobjectSlackTrackingInProgress()) {
          // Turbofan can force completion of in-object slack tracking.
          break;
        }
        CHECK_EQ(Map::kSlackTrackingCounterStart - i - 1,
                 initial_map->construction_counter());
      }
      CHECK(!initial_map->IsInobjectSlackTrackingInProgress());
      CHECK(!IsObjectShrinkable(*obj));

      // No slack left.
      CHECK_EQ(fields_count, obj->map().GetInObjectProperties());
    }
  }

  // Creates class hierarchy of length matching the |hierarchy_desc| length and
  // with the number of fields at i'th level equal to |hierarchy_desc[i]|.
  void CreateClassHierarchy(const std::vector<int>& hierarchy_desc) {
    std::ostringstream os;
    os << "'use strict';\n\n";

    int n = static_cast<int>(hierarchy_desc.size());
    for (int cur_class = 0; cur_class < n; cur_class++) {
      os << "class A" << cur_class;
      if (cur_class > 0) {
        os << " extends A" << (cur_class - 1);
      }
      os << " {\n"
            "  constructor(...args) {\n";
      if (cur_class > 0) {
        os << "    super(...args);\n";
      }
      int fields_count = hierarchy_desc[cur_class];
      for (int k = 0; k < fields_count; k++) {
        os << "    this.f" << cur_class << "_" << k << " = " << k << ";\n";
      }
      os << "  }\n"
            "};\n\n";
    }
    CompileRun(os.str().c_str());
  }

  void CheckExpectedProperties(int expected, std::ostringstream& os) {
    Handle<HeapObject> obj = Handle<HeapObject>::cast(
        v8::Utils::OpenHandle(*CompileRun(os.str().c_str())));
    CHECK_EQ(expected, obj->map().GetInObjectProperties());
  }

  void TestSubclassBuiltin(const char* subclass_name,
                           InstanceType instance_type, const char* builtin_name,
                           const char* ctor_arguments = "",
                           int builtin_properties_count = 0) {
    {
      std::ostringstream os;
      os << "'use strict';\n"
            "class "
         << subclass_name << " extends " << builtin_name
         << " {\n"
            "  constructor(...args) {\n"
            "    super(...args);\n"
            "    this.a = 42;\n"
            "    this.d = 4.2;\n"
            "    this.o = this;\n"
            "  }\n"
            "};\n";
      CompileRun(os.str().c_str());
    }

    Handle<JSFunction> func = GetLexical<JSFunction>(subclass_name);

    // Zero instances were created so far.
    CHECK(!func->has_initial_map());

    v8::Local<v8::Script> new_script;
    {
      std::ostringstream os;
      os << "new " << subclass_name << "(" << ctor_arguments << ");";
      new_script = Compile(os.str().c_str());
    }

    RunI<JSObject>(new_script);

    CHECK(func->has_initial_map());
    Handle<Map> initial_map(func->initial_map(), func->GetIsolate());

    CHECK_EQ(instance_type, initial_map->instance_type());

    // One instance of a subclass created.
    CHECK_EQ(Map::kSlackTrackingCounterStart - 1,
             initial_map->construction_counter());
    CHECK(initial_map->IsInobjectSlackTrackingInProgress());

    // Create two instances in order to ensure that |obj|.o is a data field
    // in case of Function subclassing.
    Handle<JSObject> obj = RunI<JSObject>(new_script);

    // Two instances of a subclass created.
    CHECK_EQ(Map::kSlackTrackingCounterStart - 2,
             initial_map->construction_counter());
    CHECK(initial_map->IsInobjectSlackTrackingInProgress());

    // There must be at least some slack.
    CHECK_LT(builtin_properties_count + 5, obj->map().GetInObjectProperties());
    CHECK_EQ(Smi::FromInt(42),
             GetFieldValue(*obj, builtin_properties_count + 0));
    CHECK_EQ(4.2, GetDoubleFieldValue(*obj, builtin_properties_count + 1));
    CHECK_EQ(*obj, GetFieldValue(*obj, builtin_properties_count + 2));
    CHECK(IsObjectShrinkable(*obj));

    // Create several subclass instances to complete the tracking.
    for (int i = 2; i < Map::kGenerousAllocationCount; i++) {
      CHECK(initial_map->IsInobjectSlackTrackingInProgress());
      Handle<JSObject> tmp = RunI<JSObject>(new_script);
      CHECK_EQ(initial_map->IsInobjectSlackTrackingInProgress(),
               IsObjectShrinkable(*tmp));
    }
    CHECK(!initial_map->IsInobjectSlackTrackingInProgress());
    CHECK(!IsObjectShrinkable(*obj));

    // No slack left.
    CHECK_EQ(builtin_properties_count + 3, obj->map().GetInObjectProperties());

    CHECK_EQ(instance_type, obj->map().instance_type());
  }

  void TestSubclassChain(const std::vector<int>& hierarchy_desc) {
    // Avoid eventual completion of in-object slack tracking.
    v8_flags.always_turbofan = false;

    CreateClassHierarchy(hierarchy_desc);
    TestClassHierarchy(hierarchy_desc, static_cast<int>(hierarchy_desc.size()));
  }

  void TestSubclassBasicNoBaseClassInstances() {}
};

TEST_F(InObjectSlackTrackingTest, JSObjectBasic) {
  // Avoid eventual completion of in-object slack tracking.
  v8_flags.always_turbofan = false;
  const char* source =
      "function A() {"
      "  this.a = 42;"
      "  this.d = 4.2;"
      "  this.o = this;"
      "}";
  CompileRun(source);

  Handle<JSFunction> func = GetGlobal<JSFunction>("A");

  // Zero instances were created so far.
  CHECK(!func->has_initial_map());

  v8::Local<v8::Script> new_A_script = Compile("new A();");

  Handle<JSObject> obj = RunI<JSObject>(new_A_script);

  CHECK(func->has_initial_map());
  Handle<Map> initial_map(func->initial_map(), func->GetIsolate());

  // One instance created.
  CHECK_EQ(Map::kSlackTrackingCounterStart - 1,
           initial_map->construction_counter());
  CHECK(initial_map->IsInobjectSlackTrackingInProgress());

  // There must be at least some slack.
  CHECK_LT(5, obj->map().GetInObjectProperties());
  CHECK_EQ(Smi::FromInt(42), GetFieldValue(*obj, 0));
  CHECK_EQ(4.2, GetDoubleFieldValue(*obj, 1));
  CHECK_EQ(*obj, GetFieldValue(*obj, 2));
  CHECK(IsObjectShrinkable(*obj));

  // Create several objects to complete the tracking.
  for (int i = 1; i < Map::kGenerousAllocationCount; i++) {
    CHECK(initial_map->IsInobjectSlackTrackingInProgress());
    Handle<JSObject> tmp = RunI<JSObject>(new_A_script);
    CHECK_EQ(initial_map->IsInobjectSlackTrackingInProgress(),
             IsObjectShrinkable(*tmp));
  }
  CHECK(!initial_map->IsInobjectSlackTrackingInProgress());
  CHECK(!IsObjectShrinkable(*obj));

  // No slack left.
  CHECK_EQ(3, obj->map().GetInObjectProperties());
}

TEST_F(InObjectSlackTrackingTest, JSObjectBasicNoInlineNew) {
  v8_flags.inline_new = false;
  TestJSObjectBasic();
}

TEST_F(InObjectSlackTrackingTest, JSObjectComplex) {
  // Avoid eventual completion of in-object slack tracking.
  v8_flags.always_turbofan = false;
  const char* source =
      "function A(n) {"
      "  if (n > 0) this.a = 42;"
      "  if (n > 1) this.d = 4.2;"
      "  if (n > 2) this.o1 = this;"
      "  if (n > 3) this.o2 = this;"
      "  if (n > 4) this.o3 = this;"
      "  if (n > 5) this.o4 = this;"
      "}";
  CompileRun(source);

  Handle<JSFunction> func = GetGlobal<JSFunction>("A");

  // Zero instances were created so far.
  CHECK(!func->has_initial_map());

  Handle<JSObject> obj1 = CompileRunI<JSObject>("new A(1);");
  Handle<JSObject> obj3 = CompileRunI<JSObject>("new A(3);");
  Handle<JSObject> obj5 = CompileRunI<JSObject>("new A(5);");

  CHECK(func->has_initial_map());
  Handle<Map> initial_map(func->initial_map(), func->GetIsolate());

  // Three instances created.
  CHECK_EQ(Map::kSlackTrackingCounterStart - 3,
           initial_map->construction_counter());
  CHECK(initial_map->IsInobjectSlackTrackingInProgress());

  // There must be at least some slack.
  CHECK_LT(5, obj3->map().GetInObjectProperties());
  CHECK_EQ(Smi::FromInt(42), GetFieldValue(*obj3, 0));
  CHECK_EQ(4.2, GetDoubleFieldValue(*obj3, 1));
  CHECK_EQ(*obj3, GetFieldValue(*obj3, 2));
  CHECK(IsObjectShrinkable(*obj1));
  CHECK(IsObjectShrinkable(*obj3));
  CHECK(IsObjectShrinkable(*obj5));

  // Create several objects to complete the tracking.
  for (int i = 3; i < Map::kGenerousAllocationCount; i++) {
    CHECK(initial_map->IsInobjectSlackTrackingInProgress());
    CompileRun("new A(3);");
  }
  CHECK(!initial_map->IsInobjectSlackTrackingInProgress());

  // obj1 and obj2 stays shrinkable because we don't clear unused fields.
  CHECK(IsObjectShrinkable(*obj1));
  CHECK(IsObjectShrinkable(*obj3));
  CHECK(!IsObjectShrinkable(*obj5));

  CHECK_EQ(5, obj1->map().GetInObjectProperties());
  CHECK_EQ(4, obj1->map().UnusedPropertyFields());

  CHECK_EQ(5, obj3->map().GetInObjectProperties());
  CHECK_EQ(2, obj3->map().UnusedPropertyFields());

  CHECK_EQ(5, obj5->map().GetInObjectProperties());
  CHECK_EQ(0, obj5->map().UnusedPropertyFields());

  // Since slack tracking is complete, the new objects should not be shrinkable.
  obj1 = CompileRunI<JSObject>("new A(1);");
  obj3 = CompileRunI<JSObject>("new A(3);");
  obj5 = CompileRunI<JSObject>("new A(5);");

  CHECK(!IsObjectShrinkable(*obj1));
  CHECK(!IsObjectShrinkable(*obj3));
  CHECK(!IsObjectShrinkable(*obj5));
}

TEST_F(InObjectSlackTrackingTest, JSObjectComplexNoInlineNew) {
  v8_flags.inline_new = false;
  TestJSObjectComplex();
}

TEST_F(InObjectSlackTrackingTest, JSGeneratorObjectBasic) {
  // Avoid eventual completion of in-object slack tracking.
  v8_flags.always_turbofan = false;
  const char* source =
      "function* A() {"
      "  var i = 0;"
      "  while(true) {"
      "    yield i++;"
      "  }"
      "};"
      "function CreateGenerator() {"
      "  var o = A();"
      "  o.a = 42;"
      "  o.d = 4.2;"
      "  o.o = o;"
      "  return o;"
      "}";
  CompileRun(source);

  Handle<JSFunction> func = GetGlobal<JSFunction>("A");

  // Zero instances were created so far.
  CHECK(!func->has_initial_map());

  v8::Local<v8::Script> new_A_script = Compile("CreateGenerator();");

  Handle<JSObject> obj = RunI<JSObject>(new_A_script);

  CHECK(func->has_initial_map());
  Handle<Map> initial_map(func->initial_map(), func->GetIsolate());

  // One instance created.
  CHECK_EQ(Map::kSlackTrackingCounterStart - 1,
           initial_map->construction_counter());
  CHECK(initial_map->IsInobjectSlackTrackingInProgress());

  // There must be at least some slack.
  CHECK_LT(5, obj->map().GetInObjectProperties());
  CHECK_EQ(Smi::FromInt(42), GetFieldValue(*obj, 0));
  CHECK_EQ(4.2, GetDoubleFieldValue(*obj, 1));
  CHECK_EQ(*obj, GetFieldValue(*obj, 2));
  CHECK(IsObjectShrinkable(*obj));

  // Create several objects to complete the tracking.
  for (int i = 1; i < Map::kGenerousAllocationCount; i++) {
    CHECK(initial_map->IsInobjectSlackTrackingInProgress());
    Handle<JSObject> tmp = RunI<JSObject>(new_A_script);
    CHECK_EQ(initial_map->IsInobjectSlackTrackingInProgress(),
             IsObjectShrinkable(*tmp));
  }
  CHECK(!initial_map->IsInobjectSlackTrackingInProgress());
  CHECK(!IsObjectShrinkable(*obj));

  // No slack left.
  CHECK_EQ(3, obj->map().GetInObjectProperties());
}

TEST_F(InObjectSlackTrackingTest, JSGeneratorObjectBasicNoInlineNew) {
  v8_flags.inline_new = false;
  TestJSGeneratorObjectBasic();
}

TEST_F(InObjectSlackTrackingTest, SubclassBasicNoBaseClassInstances) {
  // Avoid eventual completion of in-object slack tracking.
  v8_flags.always_turbofan = false;

  // Check that base class' and subclass' slack tracking do not interfere with
  // each other.
  // In this test we never create base class instances.

  const char* source =
      "'use strict';"
      "class A {"
      "  constructor(...args) {"
      "    this.aa = 42;"
      "    this.ad = 4.2;"
      "    this.ao = this;"
      "  }"
      "};"
      "class B extends A {"
      "  constructor(...args) {"
      "    super(...args);"
      "    this.ba = 142;"
      "    this.bd = 14.2;"
      "    this.bo = this;"
      "  }"
      "};";
  CompileRun(source);

  Handle<JSFunction> a_func = GetLexical<JSFunction>("A");
  Handle<JSFunction> b_func = GetLexical<JSFunction>("B");

  // Zero instances were created so far.
  CHECK(!a_func->has_initial_map());
  CHECK(!b_func->has_initial_map());

  v8::Local<v8::Script> new_B_script = Compile("new B();");

  Handle<JSObject> obj = RunI<JSObject>(new_B_script);

  CHECK(a_func->has_initial_map());
  Handle<Map> a_initial_map(a_func->initial_map(), a_func->GetIsolate());

  CHECK(b_func->has_initial_map());
  Handle<Map> b_initial_map(b_func->initial_map(), a_func->GetIsolate());

  // Zero instances of A created.
  CHECK_EQ(Map::kSlackTrackingCounterStart,
           a_initial_map->construction_counter());
  CHECK(a_initial_map->IsInobjectSlackTrackingInProgress());

  // One instance of B created.
  CHECK_EQ(Map::kSlackTrackingCounterStart - 1,
           b_initial_map->construction_counter());
  CHECK(b_initial_map->IsInobjectSlackTrackingInProgress());

  // There must be at least some slack.
  CHECK_LT(10, obj->map().GetInObjectProperties());
  CHECK_EQ(Smi::FromInt(42), GetFieldValue(*obj, 0));
  CHECK_EQ(4.2, GetDoubleFieldValue(*obj, 1));
  CHECK_EQ(*obj, GetFieldValue(*obj, 2));
  CHECK_EQ(Smi::FromInt(142), GetFieldValue(*obj, 3));
  CHECK_EQ(14.2, GetDoubleFieldValue(*obj, 4));
  CHECK_EQ(*obj, GetFieldValue(*obj, 5));
  CHECK(IsObjectShrinkable(*obj));

  // Create several subclass instances to complete the tracking.
  for (int i = 1; i < Map::kGenerousAllocationCount; i++) {
    CHECK(b_initial_map->IsInobjectSlackTrackingInProgress());
    Handle<JSObject> tmp = RunI<JSObject>(new_B_script);
    CHECK_EQ(b_initial_map->IsInobjectSlackTrackingInProgress(),
             IsObjectShrinkable(*tmp));
  }
  CHECK(!b_initial_map->IsInobjectSlackTrackingInProgress());
  CHECK(!IsObjectShrinkable(*obj));

  // Zero instances of A created.
  CHECK_EQ(Map::kSlackTrackingCounterStart,
           a_initial_map->construction_counter());
  CHECK(a_initial_map->IsInobjectSlackTrackingInProgress());

  // No slack left.
  CHECK_EQ(6, obj->map().GetInObjectProperties());
}

TEST_F(InObjectSlackTrackingTest,
       SubclassBasicNoBaseClassInstancesNoInlineNew) {
  v8_flags.inline_new = false;
  TestSubclassBasicNoBaseClassInstances();
}

TEST_F(InObjectSlackTrackingTest, SubclassBasic) {
  // Avoid eventual completion of in-object slack tracking.
  v8_flags.always_turbofan = false;

  // Check that base class' and subclass' slack tracking do not interfere with
  // each other.
  // In this test we first create enough base class instances to complete
  // the slack tracking and then proceed creating subclass instances.

  const char* source =
      "'use strict';"
      "class A {"
      "  constructor(...args) {"
      "    this.aa = 42;"
      "    this.ad = 4.2;"
      "    this.ao = this;"
      "  }"
      "};"
      "class B extends A {"
      "  constructor(...args) {"
      "    super(...args);"
      "    this.ba = 142;"
      "    this.bd = 14.2;"
      "    this.bo = this;"
      "  }"
      "};";
  CompileRun(source);

  Handle<JSFunction> a_func = GetLexical<JSFunction>("A");
  Handle<JSFunction> b_func = GetLexical<JSFunction>("B");

  // Zero instances were created so far.
  CHECK(!a_func->has_initial_map());
  CHECK(!b_func->has_initial_map());

  v8::Local<v8::Script> new_A_script = Compile("new A();");
  v8::Local<v8::Script> new_B_script = Compile("new B();");

  Handle<JSObject> a_obj = RunI<JSObject>(new_A_script);
  Handle<JSObject> b_obj = RunI<JSObject>(new_B_script);

  CHECK(a_func->has_initial_map());
  Handle<Map> a_initial_map(a_func->initial_map(), a_func->GetIsolate());

  CHECK(b_func->has_initial_map());
  Handle<Map> b_initial_map(b_func->initial_map(), a_func->GetIsolate());

  // One instance of a base class created.
  CHECK_EQ(Map::kSlackTrackingCounterStart - 1,
           a_initial_map->construction_counter());
  CHECK(a_initial_map->IsInobjectSlackTrackingInProgress());

  // One instance of a subclass created.
  CHECK_EQ(Map::kSlackTrackingCounterStart - 1,
           b_initial_map->construction_counter());
  CHECK(b_initial_map->IsInobjectSlackTrackingInProgress());

  // Create several base class instances to complete the tracking.
  for (int i = 1; i < Map::kGenerousAllocationCount; i++) {
    CHECK(a_initial_map->IsInobjectSlackTrackingInProgress());
    Handle<JSObject> tmp = RunI<JSObject>(new_A_script);
    CHECK_EQ(a_initial_map->IsInobjectSlackTrackingInProgress(),
             IsObjectShrinkable(*tmp));
  }
  CHECK(!a_initial_map->IsInobjectSlackTrackingInProgress());
  CHECK(!IsObjectShrinkable(*a_obj));

  // No slack left.
  CHECK_EQ(3, a_obj->map().GetInObjectProperties());

  // There must be at least some slack.
  CHECK_LT(10, b_obj->map().GetInObjectProperties());
  CHECK_EQ(Smi::FromInt(42), GetFieldValue(*b_obj, 0));
  CHECK_EQ(4.2, GetDoubleFieldValue(*b_obj, 1));
  CHECK_EQ(*b_obj, GetFieldValue(*b_obj, 2));
  CHECK_EQ(Smi::FromInt(142), GetFieldValue(*b_obj, 3));
  CHECK_EQ(14.2, GetDoubleFieldValue(*b_obj, 4));
  CHECK_EQ(*b_obj, GetFieldValue(*b_obj, 5));
  CHECK(IsObjectShrinkable(*b_obj));

  // Create several subclass instances to complete the tracking.
  for (int i = 1; i < Map::kGenerousAllocationCount; i++) {
    CHECK(b_initial_map->IsInobjectSlackTrackingInProgress());
    Handle<JSObject> tmp = RunI<JSObject>(new_B_script);
    CHECK_EQ(b_initial_map->IsInobjectSlackTrackingInProgress(),
             IsObjectShrinkable(*tmp));
  }
  CHECK(!b_initial_map->IsInobjectSlackTrackingInProgress());
  CHECK(!IsObjectShrinkable(*b_obj));

  // No slack left.
  CHECK_EQ(6, b_obj->map().GetInObjectProperties());
}

TEST_F(InObjectSlackTrackingTest, SubclassBasicNoInlineNew) {
  v8_flags.inline_new = false;
  TestSubclassBasic();
}

TEST_F(InObjectSlackTrackingTest, Subclasses) {
  std::vector<int> hierarchy_desc;
  hierarchy_desc.push_back(50);
  hierarchy_desc.push_back(128);
  TestSubclassChain(hierarchy_desc);
}

TEST_F(InObjectSlackTrackingTest, LongSubclassChain1) {
  std::vector<int> hierarchy_desc;
  for (int i = 0; i < 7; i++) {
    hierarchy_desc.push_back(i * 10);
  }
  TestSubclassChain(hierarchy_desc);
}

TEST_F(InObjectSlackTrackingTest, LongSubclassChain2) {
  std::vector<int> hierarchy_desc;
  hierarchy_desc.push_back(10);
  for (int i = 0; i < 42; i++) {
    hierarchy_desc.push_back(0);
  }
  hierarchy_desc.push_back(230);
  TestSubclassChain(hierarchy_desc);
}

TEST_F(InObjectSlackTrackingTest, LongSubclassChain3) {
  std::vector<int> hierarchy_desc;
  for (int i = 0; i < 42; i++) {
    hierarchy_desc.push_back(5);
  }
  TestSubclassChain(hierarchy_desc);
}

TEST_F(InObjectSlackTrackingTest, InobjectPropetiesCountOverflowInSubclass) {
  // Avoid eventual completion of in-object slack tracking.
  v8_flags.always_turbofan = false;

  std::vector<int> hierarchy_desc;
  const int kNoOverflowCount = 5;
  for (int i = 0; i < kNoOverflowCount; i++) {
    hierarchy_desc.push_back(50);
  }
  // In this class we are going to have properties in the backing store.
  hierarchy_desc.push_back(100);

  CreateClassHierarchy(hierarchy_desc);

  // For the last class in the hierarchy we need different checks.
  {
    int cur_class = kNoOverflowCount;
    std::string class_name = GetClassName(cur_class);

    // Create |class_name| objects and check slack tracking.
    v8::Local<v8::Script> new_script = GetNewObjectScript(class_name);

    Handle<JSFunction> func = GetLexical<JSFunction>(class_name);

    Handle<JSObject> obj = RunI<JSObject>(new_script);

    CHECK(func->has_initial_map());
    Handle<Map> initial_map(func->initial_map(), func->GetIsolate());

    // There must be no slack left.
    CHECK_EQ(JSObject::kMaxInstanceSize, obj->map().instance_size());
    CHECK_EQ(kMaxInobjectProperties, obj->map().GetInObjectProperties());

    // One instance was created.
    CHECK_EQ(Map::kSlackTrackingCounterStart - 1,
             initial_map->construction_counter());
    CHECK(initial_map->IsInobjectSlackTrackingInProgress());

    // Create several instances to complete the tracking.
    for (int i = 1; i < Map::kGenerousAllocationCount; i++) {
      CHECK(initial_map->IsInobjectSlackTrackingInProgress());
      Handle<JSObject> tmp = RunI<JSObject>(new_script);
      CHECK(!IsObjectShrinkable(*tmp));
    }
    CHECK(!initial_map->IsInobjectSlackTrackingInProgress());
    CHECK(!IsObjectShrinkable(*obj));

    // No slack left.
    CHECK_EQ(kMaxInobjectProperties, obj->map().GetInObjectProperties());
  }

  // The other classes in the hierarchy are not affected.
  TestClassHierarchy(hierarchy_desc, kNoOverflowCount);
}

TEST_F(InObjectSlackTrackingTest, ObjectLiteralPropertyBackingStoreSize) {
  std::ostringstream os;

  // An index key does not require space in the property backing store.
  os << "(function() {\n"
        "  function f() {\n"
        "    var o = {\n"
        "      '-1': 42,\n"  // Allocate for non-index key.
        "      1: 42,\n"     // Do not allocate for index key.
        "      '2': 42\n"    // Do not allocate for index key.
        "    };\n"
        "    return o;\n"
        "  }\n"
        "\n"
        "  return f();\n"
        "} )();";
  CheckExpectedProperties(1, os);

  // Avoid over-/under-allocation for computed property names.
  os << "(function() {\n"
        "  'use strict';\n"
        "  function f(x) {\n"
        "    var o = {\n"
        "      1: 42,\n"    // Do not allocate for index key.
        "      '2': 42,\n"  // Do not allocate for index key.
        "      [x]: 42,\n"  // Allocate for property with computed name.
        "      3: 42,\n"    // Do not allocate for index key.
        "      '4': 42\n"   // Do not allocate for index key.
        "    };\n"
        "    return o;\n"
        "  }\n"
        "\n"
        "  var x = 'hello'\n"
        "\n"
        "  return f(x);\n"
        "} )();";
  CheckExpectedProperties(1, os);

  // Conversion to index key.
  os << "(function() {\n"
        "  function f(x) {\n"
        "    var o = {\n"
        "      1: 42,\n"       // Do not allocate for index key.
        "      '2': 42,\n"     // Do not allocate for index key.
        "      [x]: 42,\n"     // Allocate for property with computed name.
        "      3: 42,\n"       // Do not allocate for index key.
        "      get 12() {}\n"  // Do not allocate for index key.
        "    };\n"
        "    return o;\n"
        "  }\n"
        "\n"
        "  var x = 'hello'\n"
        "\n"
        "  return f(x);\n"
        "} )();";
  CheckExpectedProperties(1, os);

  os << "(function() {\n"
        "  function f() {\n"
        "    var o = {};\n"
        "    return o;\n"
        "  }\n"
        "\n"
        "  return f();\n"
        "} )();";
  // Empty objects have slack for 4 properties.
  CheckExpectedProperties(4, os);

  os << "(function() {\n"
        "  function f(x) {\n"
        "    var o = {\n"
        "      a: 42,\n"    // Allocate for constant property.
        "      [x]: 42,\n"  // Allocate for property with computed name.
        "      b: 42\n"     // Allocate for constant property.
        "    };\n"
        "    return o;\n"
        "  }\n"
        "\n"
        "  var x = 'hello'\n"
        "\n"
        "  return f(x);\n"
        "} )();";
  CheckExpectedProperties(3, os);

  os << "(function() {\n"
        "  function f(x) {\n"
        "    var o = {\n"
        "      a: 42,\n"          // Allocate for constant property.
        "      __proto__: 42,\n"  // Do not allocate for __proto__.
        "      [x]: 42\n"         // Allocate for property with computed name.
        "    };\n"
        "    return o;\n"
        "  }\n"
        "\n"
        "  var x = 'hello'\n"
        "\n"
        "  return f(x);\n"
        "} )();";
  // __proto__ is not allocated in the backing store.
  CheckExpectedProperties(2, os);

  os << "(function() {\n"
        "  function f(x) {\n"
        "    var o = {\n"
        "      a: 42,\n"         // Allocate for constant property.
        "      [x]: 42,\n"       // Allocate for property with computed name.
        "      __proto__: 42\n"  // Do not allocate for __proto__.
        "    };\n"
        "    return o;\n"
        "  }\n"
        "\n"
        "  var x = 'hello'\n"
        "\n"
        "  return f(x);\n"
        "} )();";
  CheckExpectedProperties(2, os);
}

TEST_F(InObjectSlackTrackingTest, SlowModeSubclass) {
  if (v8_flags.stress_concurrent_allocation) return;

  // Avoid eventual completion of in-object slack tracking.
  v8_flags.always_turbofan = false;

  std::vector<int> hierarchy_desc;
  const int kNoOverflowCount = 5;
  for (int i = 0; i < kNoOverflowCount; i++) {
    hierarchy_desc.push_back(50);
  }
  // This class should go dictionary mode.
  hierarchy_desc.push_back(1000);

  CreateClassHierarchy(hierarchy_desc);

  // For the last class in the hierarchy we need different checks.
  {
    int cur_class = kNoOverflowCount;
    std::string class_name = GetClassName(cur_class);

    // Create |class_name| objects and check slack tracking.
    v8::Local<v8::Script> new_script = GetNewObjectScript(class_name);

    Handle<JSFunction> func = GetLexical<JSFunction>(class_name);

    Handle<JSObject> obj = RunI<JSObject>(new_script);

    CHECK(func->has_initial_map());
    Handle<Map> initial_map(func->initial_map(), func->GetIsolate());

    // Object should go dictionary mode.
    CHECK_EQ(JSObject::kHeaderSize, obj->map().instance_size());
    CHECK(obj->map().is_dictionary_map());

    // One instance was created.
    CHECK_EQ(Map::kSlackTrackingCounterStart - 1,
             initial_map->construction_counter());
    CHECK(initial_map->IsInobjectSlackTrackingInProgress());

    // Create several instances to complete the tracking.
    for (int i = 1; i < Map::kGenerousAllocationCount; i++) {
      CHECK(initial_map->IsInobjectSlackTrackingInProgress());
      Handle<JSObject> tmp = RunI<JSObject>(new_script);
      CHECK(!IsObjectShrinkable(*tmp));
    }
    CHECK(!initial_map->IsInobjectSlackTrackingInProgress());
    CHECK(!IsObjectShrinkable(*obj));

    // Object should stay in dictionary mode.
    CHECK_EQ(JSObject::kHeaderSize, obj->map().instance_size());
    CHECK(obj->map().is_dictionary_map());
  }

  // The other classes in the hierarchy are not affected.
  TestClassHierarchy(hierarchy_desc, kNoOverflowCount);
}

TEST_F(InObjectSlackTrackingTest, SubclassObjectBuiltin) {
  // Avoid eventual completion of in-object slack tracking.
  v8_flags.always_turbofan = false;
  TestSubclassBuiltin("A1", JS_OBJECT_TYPE, "Object", "true");
  TestSubclassBuiltin("A2", JS_OBJECT_TYPE, "Object", "42");
  TestSubclassBuiltin("A3", JS_OBJECT_TYPE, "Object", "'some string'");
}

TEST_F(InObjectSlackTrackingTest, SubclassObjectBuiltinNoInlineNew) {
  v8_flags.inline_new = false;
  TestSubclassObjectBuiltin();
}

TEST_F(InObjectSlackTrackingTest, SubclassFunctionBuiltin) {
  // Avoid eventual completion of in-object slack tracking.
  v8_flags.always_turbofan = false;

  TestSubclassBuiltin("A1", JS_FUNCTION_TYPE, "Function", "'return 153;'");
  TestSubclassBuiltin("A2", JS_FUNCTION_TYPE, "Function", "'this.a = 44;'");
}

TEST_F(InObjectSlackTrackingTest, SubclassFunctionBuiltinNoInlineNew) {
  v8_flags.inline_new = false;
  TestSubclassFunctionBuiltin();
}

TEST_F(InObjectSlackTrackingTest, SubclassBooleanBuiltin) {
  // Avoid eventual completion of in-object slack tracking.
  v8_flags.always_turbofan = false;

  TestSubclassBuiltin("A1", JS_PRIMITIVE_WRAPPER_TYPE, "Boolean", "true");
  TestSubclassBuiltin("A2", JS_PRIMITIVE_WRAPPER_TYPE, "Boolean", "false");
}

TEST_F(InObjectSlackTrackingTest, SubclassBooleanBuiltinNoInlineNew) {
  v8_flags.inline_new = false;
  TestSubclassBooleanBuiltin();
}

TEST_F(InObjectSlackTrackingTest, SubclassErrorBuiltin) {
  // Avoid eventual completion of in-object slack tracking.
  v8_flags.always_turbofan = false;

  const int first_field = 2;
  TestSubclassBuiltin("A1", JS_ERROR_TYPE, "Error", "'err'", first_field);
  TestSubclassBuiltin("A2", JS_ERROR_TYPE, "EvalError", "'err'", first_field);
  TestSubclassBuiltin("A3", JS_ERROR_TYPE, "RangeError", "'err'", first_field);
  TestSubclassBuiltin("A4", JS_ERROR_TYPE, "ReferenceError", "'err'",
                      first_field);
  TestSubclassBuiltin("A5", JS_ERROR_TYPE, "SyntaxError", "'err'", first_field);
  TestSubclassBuiltin("A6", JS_ERROR_TYPE, "TypeError", "'err'", first_field);
  TestSubclassBuiltin("A7", JS_ERROR_TYPE, "URIError", "'err'", first_field);
}

TEST_F(InObjectSlackTrackingTest, SubclassErrorBuiltinNoInlineNew) {
  v8_flags.inline_new = false;
  TestSubclassErrorBuiltin();
}

TEST_F(InObjectSlackTrackingTest, SubclassNumberBuiltin) {
  // Avoid eventual completion of in-object slack tracking.
  v8_flags.always_turbofan = false;

  TestSubclassBuiltin("A1", JS_PRIMITIVE_WRAPPER_TYPE, "Number", "42");
  TestSubclassBuiltin("A2", JS_PRIMITIVE_WRAPPER_TYPE, "Number", "4.2");
}

TEST_F(InObjectSlackTrackingTest, SubclassNumberBuiltinNoInlineNew) {
  v8_flags.inline_new = false;
  TestSubclassNumberBuiltin();
}

TEST_F(InObjectSlackTrackingTest, SubclassDateBuiltin) {
  // Avoid eventual completion of in-object slack tracking.
  v8_flags.always_turbofan = false;

  TestSubclassBuiltin("A1", JS_DATE_TYPE, "Date", "123456789");
}

TEST_F(InObjectSlackTrackingTest, SubclassDateBuiltinNoInlineNew) {
  v8_flags.inline_new = false;
  TestSubclassDateBuiltin();
}

TEST_F(InObjectSlackTrackingTest, SubclassStringBuiltin) {
  // Avoid eventual completion of in-object slack tracking.
  v8_flags.always_turbofan = false;

  TestSubclassBuiltin("A1", JS_PRIMITIVE_WRAPPER_TYPE, "String",
                      "'some string'");
  TestSubclassBuiltin("A2", JS_PRIMITIVE_WRAPPER_TYPE, "String", "");
}

TEST_F(InObjectSlackTrackingTest, SubclassStringBuiltinNoInlineNew) {
  v8_flags.inline_new = false;
}

TEST_F(InObjectSlackTrackingTest, SubclassRegExpBuiltin) {
  // Avoid eventual completion of in-object slack tracking.
  v8_flags.always_turbofan = false;

  const int first_field = 1;
  TestSubclassBuiltin("A1", JS_REG_EXP_TYPE, "RegExp", "'o(..)h', 'g'",
                      first_field);
}

TEST_F(InObjectSlackTrackingTest, SubclassRegExpBuiltinNoInlineNew) {
  v8_flags.inline_new = false;
  TestSubclassRegExpBuiltin();
}

TEST_F(InObjectSlackTrackingTest, SubclassArrayBuiltin) {
  // Avoid eventual completion of in-object slack tracking.
  v8_flags.always_turbofan = false;

  TestSubclassBuiltin("A1", JS_ARRAY_TYPE, "Array", "42");
}

TEST_F(InObjectSlackTrackingTest, SubclassArrayBuiltinNoInlineNew) {
  v8_flags.inline_new = false;
  TestSubclassArrayBuiltin();
}

TEST_F(InObjectSlackTrackingTest, SubclassTypedArrayBuiltin) {
  // Avoid eventual completion of in-object slack tracking.
  v8_flags.always_turbofan = false;

#define TYPED_ARRAY_TEST_F(InObjectSlackTrackingTest, Type, type, TYPE, \
                           elementType)                                 \
  TestSubclassBuiltin("A" #Type, JS_TYPED_ARRAY_TYPE, #Type "Array", "42");

  TYPED_ARRAYS(TYPED_ARRAY_TEST)

#undef TYPED_ARRAY_TEST
}

TEST_F(InObjectSlackTrackingTest, SubclassTypedArrayBuiltinNoInlineNew) {
  v8_flags.inline_new = false;
  TestSubclassTypedArrayBuiltin();
}

TEST_F(InObjectSlackTrackingTest, SubclassCollectionBuiltin) {
  // Avoid eventual completion of in-object slack tracking.
  v8_flags.always_turbofan = false;
  CcTest::InitializeVM();
  v8::HandleScope scope(CcTest::isolate());

  TestSubclassBuiltin("A1", JS_SET_TYPE, "Set", "");
  TestSubclassBuiltin("A2", JS_MAP_TYPE, "Map", "");
  TestSubclassBuiltin("A3", JS_WEAK_SET_TYPE, "WeakSet", "");
  TestSubclassBuiltin("A4", JS_WEAK_MAP_TYPE, "WeakMap", "");
}

TEST_F(InObjectSlackTrackingTest, SubclassCollectionBuiltinNoInlineNew) {
  v8_flags.inline_new = false;
  TestSubclassCollectionBuiltin();
}

TEST_F(InObjectSlackTrackingTest, SubclassArrayBufferBuiltin) {
  // Avoid eventual completion of in-object slack tracking.
  v8_flags.always_turbofan = false;
  CcTest::InitializeVM();
  v8::HandleScope scope(CcTest::isolate());

  TestSubclassBuiltin("A1", JS_ARRAY_BUFFER_TYPE, "ArrayBuffer", "42");
  TestSubclassBuiltin("A2", JS_DATA_VIEW_TYPE, "DataView",
                      "new ArrayBuffer(42)");
}

TEST_F(InObjectSlackTrackingTest, SubclassArrayBufferBuiltinNoInlineNew) {
  v8_flags.inline_new = false;
  TestSubclassArrayBufferBuiltin();
}

TEST_F(InObjectSlackTrackingTest, SubclassPromiseBuiltin) {
  // Avoid eventual completion of in-object slack tracking.
  v8_flags.always_turbofan = false;
  CcTest::InitializeVM();
  v8::HandleScope scope(CcTest::isolate());

  TestSubclassBuiltin("A1", JS_PROMISE_TYPE, "Promise",
                      "function(resolve, reject) { resolve('ok'); }");
}

TEST_F(InObjectSlackTrackingTest, SubclassPromiseBuiltinNoInlineNew) {
  v8_flags.inline_new = false;
  TestSubclassPromiseBuiltin();
}

TEST_F(InObjectSlackTrackingTest, SubclassTranspiledClassHierarchy) {
  CcTest::InitializeVM();
  v8::HandleScope scope(CcTest::isolate());

  CompileRun(
      "Object.setPrototypeOf(B, A);\n"
      "function A() {\n"
      "  this.a0 = 0;\n"
      "  this.a1 = 1;\n"
      "  this.a2 = 1;\n"
      "  this.a3 = 1;\n"
      "  this.a4 = 1;\n"
      "  this.a5 = 1;\n"
      "  this.a6 = 1;\n"
      "  this.a7 = 1;\n"
      "  this.a8 = 1;\n"
      "  this.a9 = 1;\n"
      "  this.a10 = 1;\n"
      "  this.a11 = 1;\n"
      "  this.a12 = 1;\n"
      "  this.a13 = 1;\n"
      "  this.a14 = 1;\n"
      "  this.a15 = 1;\n"
      "  this.a16 = 1;\n"
      "  this.a17 = 1;\n"
      "  this.a18 = 1;\n"
      "  this.a19 = 1;\n"
      "};\n"
      "function B() {\n"
      "  A.call(this);\n"
      "  this.b = 1;\n"
      "};\n");

  Handle<JSFunction> func = GetGlobal<JSFunction>("B");

  // Zero instances have been created so far.
  CHECK(!func->has_initial_map());

  v8::Local<v8::Script> new_script = v8_compile("new B()");

  RunI<JSObject>(new_script);

  CHECK(func->has_initial_map());
  Handle<Map> initial_map(func->initial_map(), func->GetIsolate());

  CHECK_EQ(JS_OBJECT_TYPE, initial_map->instance_type());

  // One instance of a subclass created.
  CHECK_EQ(Map::kSlackTrackingCounterStart - 1,
           initial_map->construction_counter());
  CHECK(initial_map->IsInobjectSlackTrackingInProgress());

  // Create two instances in order to ensure that |obj|.o is a data field
  // in case of Function subclassing.
  Handle<JSObject> obj = RunI<JSObject>(new_script);

  // Two instances of a subclass created.
  CHECK_EQ(Map::kSlackTrackingCounterStart - 2,
           initial_map->construction_counter());
  CHECK(initial_map->IsInobjectSlackTrackingInProgress());
  CHECK(IsObjectShrinkable(*obj));

  // Create several subclass instances to complete the tracking.
  for (int i = 2; i < Map::kGenerousAllocationCount; i++) {
    CHECK(initial_map->IsInobjectSlackTrackingInProgress());
    Handle<JSObject> tmp = RunI<JSObject>(new_script);
    CHECK_EQ(initial_map->IsInobjectSlackTrackingInProgress(),
             IsObjectShrinkable(*tmp));
  }
  CHECK(!initial_map->IsInobjectSlackTrackingInProgress());
  CHECK(!IsObjectShrinkable(*obj));

  // No slack left.
  CHECK_EQ(21, obj->map().GetInObjectProperties());
  CHECK_EQ(JS_OBJECT_TYPE, obj->map().instance_type());
}

TEST_F(InObjectSlackTrackingTest, Regress8853_ClassConstructor) {
  CcTest::InitializeVM();
  v8::HandleScope scope(CcTest::isolate());

  // For classes without any this.prop assignments in their
  // constructors we start out with 10 inobject properties.
  Handle<JSObject> obj = CompileRunI<JSObject>("new (class {});\n");
  CHECK(obj->map().IsInobjectSlackTrackingInProgress());
  CHECK(IsObjectShrinkable(*obj));
  CHECK_EQ(10, obj->map().GetInObjectProperties());

  // For classes with N explicit this.prop assignments in their
  // constructors we start out with N+8 inobject properties.
  obj = CompileRunI<JSObject>(
      "new (class {\n"
      "  constructor() {\n"
      "    this.x = 1;\n"
      "    this.y = 2;\n"
      "    this.z = 3;\n"
      "  }\n"
      "});\n");
  CHECK(obj->map().IsInobjectSlackTrackingInProgress());
  CHECK(IsObjectShrinkable(*obj));
  CHECK_EQ(3 + 8, obj->map().GetInObjectProperties());
}

TEST_F(InObjectSlackTrackingTest, Regress8853_ClassHierarchy) {
  CcTest::InitializeVM();
  v8::HandleScope scope(CcTest::isolate());

  // For class hierarchies without any this.prop assignments in their
  // constructors we reserve 2 inobject properties per constructor plus
  // 8 inobject properties slack on top.
  std::string base = "(class {})";
  for (int i = 1; i < 10; ++i) {
    std::string script = "new " + base + ";\n";
    Handle<JSObject> obj = CompileRunI<JSObject>(script.c_str());
    CHECK(obj->map().IsInobjectSlackTrackingInProgress());
    CHECK(IsObjectShrinkable(*obj));
    CHECK_EQ(8 + 2 * i, obj->map().GetInObjectProperties());
    base = "(class extends " + base + " {})";
  }
}

TEST_F(InObjectSlackTrackingTest, Regress8853_FunctionConstructor) {
  CcTest::InitializeVM();
  v8::HandleScope scope(CcTest::isolate());

  // For constructor functions without any this.prop assignments in
  // them we start out with 10 inobject properties.
  Handle<JSObject> obj = CompileRunI<JSObject>("new (function() {});\n");
  CHECK(obj->map().IsInobjectSlackTrackingInProgress());
  CHECK(IsObjectShrinkable(*obj));
  CHECK_EQ(10, obj->map().GetInObjectProperties());

  // For constructor functions with N explicit this.prop assignments
  // in them we start out with N+8 inobject properties.
  obj = CompileRunI<JSObject>(
      "new (function() {\n"
      "  this.a = 1;\n"
      "  this.b = 2;\n"
      "  this.c = 3;\n"
      "  this.d = 3;\n"
      "  this.c = 3;\n"
      "  this.f = 3;\n"
      "});\n");
  CHECK(obj->map().IsInobjectSlackTrackingInProgress());
  CHECK(IsObjectShrinkable(*obj));
  CHECK_EQ(6 + 8, obj->map().GetInObjectProperties());
}

TEST_F(InObjectSlackTrackingTest,
       InstanceFieldsArePropertiesDefaultConstructorLazy) {
  CcTest::InitializeVM();
  v8::HandleScope scope(CcTest::isolate());

  Handle<JSObject> obj = CompileRunI<JSObject>(
      "new (class {\n"
      "  x00 = null;\n"
      "  x01 = null;\n"
      "  x02 = null;\n"
      "  x03 = null;\n"
      "  x04 = null;\n"
      "  x05 = null;\n"
      "  x06 = null;\n"
      "  x07 = null;\n"
      "  x08 = null;\n"
      "  x09 = null;\n"
      "  x10 = null;\n"
      "});\n");
  CHECK_EQ(11 + 8, obj->map().GetInObjectProperties());
}

TEST_F(InObjectSlackTrackingTest,
       InstanceFieldsArePropertiesFieldsAndConstructorLazy) {
  CcTest::InitializeVM();
  v8::HandleScope scope(CcTest::isolate());

  Handle<JSObject> obj = CompileRunI<JSObject>(
      "new (class {\n"
      "  x00 = null;\n"
      "  x01 = null;\n"
      "  x02 = null;\n"
      "  x03 = null;\n"
      "  x04 = null;\n"
      "  x05 = null;\n"
      "  x06 = null;\n"
      "  x07 = null;\n"
      "  x08 = null;\n"
      "  x09 = null;\n"
      "  x10 = null;\n"
      "  constructor() {\n"
      "    this.x11 = null;\n"
      "    this.x12 = null;\n"
      "    this.x12 = null;\n"
      "    this.x14 = null;\n"
      "    this.x15 = null;\n"
      "    this.x16 = null;\n"
      "    this.x17 = null;\n"
      "    this.x18 = null;\n"
      "    this.x19 = null;\n"
      "    this.x20 = null;\n"
      "  }\n"
      "});\n");
  CHECK_EQ(21 + 8, obj->map().GetInObjectProperties());
}

TEST_F(InObjectSlackTrackingTest,
       InstanceFieldsArePropertiesDefaultConstructorEager) {
  i::v8_flags.lazy = false;
  CcTest::InitializeVM();
  v8::HandleScope scope(CcTest::isolate());

  Handle<JSObject> obj = CompileRunI<JSObject>(
      "new (class {\n"
      "  x00 = null;\n"
      "  x01 = null;\n"
      "  x02 = null;\n"
      "  x03 = null;\n"
      "  x04 = null;\n"
      "  x05 = null;\n"
      "  x06 = null;\n"
      "  x07 = null;\n"
      "  x08 = null;\n"
      "  x09 = null;\n"
      "  x10 = null;\n"
      "});\n");
  CHECK_EQ(11 + 8, obj->map().GetInObjectProperties());
}

TEST_F(InObjectSlackTrackingTest,
       InstanceFieldsArePropertiesFieldsAndConstructorEager) {
  i::v8_flags.lazy = false;
  CcTest::InitializeVM();
  v8::HandleScope scope(CcTest::isolate());

  Handle<JSObject> obj = CompileRunI<JSObject>(
      "new (class {\n"
      "  x00 = null;\n"
      "  x01 = null;\n"
      "  x02 = null;\n"
      "  x03 = null;\n"
      "  x04 = null;\n"
      "  x05 = null;\n"
      "  x06 = null;\n"
      "  x07 = null;\n"
      "  x08 = null;\n"
      "  x09 = null;\n"
      "  x10 = null;\n"
      "  constructor() {\n"
      "    this.x11 = null;\n"
      "    this.x12 = null;\n"
      "    this.x12 = null;\n"
      "    this.x14 = null;\n"
      "    this.x15 = null;\n"
      "    this.x16 = null;\n"
      "    this.x17 = null;\n"
      "    this.x18 = null;\n"
      "    this.x19 = null;\n"
      "    this.x20 = null;\n"
      "  }\n"
      "});\n");
  CHECK_EQ(21 + 8, obj->map().GetInObjectProperties());
}

}  // namespace test_inobject_slack_tracking
}  // namespace internal
}  // namespace v8
