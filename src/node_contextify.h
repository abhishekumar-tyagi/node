#ifndef SRC_NODE_CONTEXTIFY_H_
#define SRC_NODE_CONTEXTIFY_H_

#include "node_internals.h"
#include "node_watchdog.h"
#include "base_object-inl.h"
#include "node_context_data.h"

namespace node {
namespace contextify {

struct ContextOptions {
  v8::Local<v8::String> name;
  v8::Local<v8::String> origin;
  v8::Local<v8::Boolean> allow_code_gen_strings;
  v8::Local<v8::Boolean> allow_code_gen_wasm;
};

enum SourceType { kScript, kModule };

class ContextifyContext {
 protected:
  Environment* const env_;
  Persistent<v8::Context> context_;

 public:
  ContextifyContext(Environment* env,
                    v8::Local<v8::Object> sandbox_obj,
                    const ContextOptions& options);

  v8::Local<v8::Value> CreateDataWrapper(Environment* env);
  v8::Local<v8::Context> CreateV8Context(Environment* env,
      v8::Local<v8::Object> sandbox_obj, const ContextOptions& options);
  static void Init(Environment* env, v8::Local<v8::Object> target);

  static bool AllowWasmCodeGeneration(
      v8::Local<v8::Context> context, v8::Local<v8::String>);

  static ContextifyContext* ContextFromContextifiedSandbox(
      Environment* env,
      const v8::Local<v8::Object>& sandbox);

  inline Environment* env() const {
    return env_;
  }

  inline v8::Local<v8::Context> context() const {
    return PersistentToLocal(env()->isolate(), context_);
  }

  inline v8::Local<v8::Object> global_proxy() const {
    return context()->Global();
  }

  inline v8::Local<v8::Object> sandbox() const {
    return v8::Local<v8::Object>::Cast(
        context()->GetEmbedderData(ContextEmbedderIndex::kSandboxObject));
  }

 private:
  static void MakeContext(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void IsContext(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void WeakCallback(
      const v8::WeakCallbackInfo<ContextifyContext>& data);
  static void PropertyGetterCallback(
      v8::Local<v8::Name> property,
      const v8::PropertyCallbackInfo<v8::Value>& args);
  static void PropertySetterCallback(
      v8::Local<v8::Name> property,
      v8::Local<v8::Value> value,
      const v8::PropertyCallbackInfo<v8::Value>& args);
  static void PropertyDescriptorCallback(
      v8::Local<v8::Name> property,
      const v8::PropertyCallbackInfo<v8::Value>& args);
  static void PropertyDefinerCallback(
      v8::Local<v8::Name> property,
      const v8::PropertyDescriptor& desc,
      const v8::PropertyCallbackInfo<v8::Value>& args);
  static void PropertyDeleterCallback(
      v8::Local<v8::Name> property,
      const v8::PropertyCallbackInfo<v8::Boolean>& args);
  static void PropertyEnumeratorCallback(
      const v8::PropertyCallbackInfo<v8::Array>& args);
  static void IndexedPropertyGetterCallback(
      uint32_t index,
      const v8::PropertyCallbackInfo<v8::Value>& args);
  static void IndexedPropertySetterCallback(
      uint32_t index,
      v8::Local<v8::Value> value,
      const v8::PropertyCallbackInfo<v8::Value>& args);
  static void IndexedPropertyDescriptorCallback(
      uint32_t index,
      const v8::PropertyCallbackInfo<v8::Value>& args);
  static void IndexedPropertyDefinerCallback(
      uint32_t index,
      const v8::PropertyDescriptor& desc,
      const v8::PropertyCallbackInfo<v8::Value>& args);
  static void IndexedPropertyDeleterCallback(
      uint32_t index,
      const v8::PropertyCallbackInfo<v8::Boolean>& args);
};

class ContextifyScript : public BaseObject {
 private:
  Persistent<v8::UnboundScript> script_;

  static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
  static bool InstanceOf(Environment* env, const v8::Local<v8::Value>& value);
  static void RunInThisContext(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void RunInContext(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void DecorateErrorStack(
      Environment* env,
      const v8::TryCatch& try_catch);
  static bool EvalMachine(
      Environment* env,
      const int64_t timeout,
      const bool display_errors,
      const bool break_on_sigint,
      const v8::FunctionCallbackInfo<v8::Value>& args);

 public:
  static void Init(Environment* env, v8::Local<v8::Object> target);
  static ContextifyScript* GetFromID(Environment* env, int id);

  ContextifyScript(Environment* env, v8::Local<v8::Object> object)
    : BaseObject(env, object) {
      MakeWeak<ContextifyScript>(this);
  }

  ~ContextifyScript() {
    env()->id_to_script_wrap_map.erase(GetID());
  }

  inline int GetID() { return script_.Get(env()->isolate())->GetId(); }
};

}  // namespace contextify
}  // namespace node

#endif  // SRC_NODE_CONTEXTIFY_H_
