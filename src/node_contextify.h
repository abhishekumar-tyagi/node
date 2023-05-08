#ifndef SRC_NODE_CONTEXTIFY_H_
#define SRC_NODE_CONTEXTIFY_H_

#if defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS

#include "base_object-inl.h"
#include "node_context_data.h"
#include "node_errors.h"
#include "memory_tracker-inl.h"

namespace node {
class ExternalReferenceRegistry;

namespace contextify {

class MicrotaskQueueWrap : public BaseObject {
 public:
  MicrotaskQueueWrap(Environment* env, v8::Local<v8::Object> obj);

  const std::shared_ptr<v8::MicrotaskQueue>& microtask_queue() const;

  static void CreatePerIsolateProperties(IsolateData* isolate_data,
                                         v8::Local<v8::ObjectTemplate> target);
  static void RegisterExternalReferences(ExternalReferenceRegistry* registry);
  static void New(const v8::FunctionCallbackInfo<v8::Value>& args);

  // This could have methods for running the microtask queue, if we ever decide
  // to make that fully customizable from userland.

  SET_NO_MEMORY_INFO()
  SET_MEMORY_INFO_NAME(MicrotaskQueueWrap)
  SET_SELF_SIZE(MicrotaskQueueWrap)

 private:
  std::shared_ptr<v8::MicrotaskQueue> microtask_queue_;
};

struct ContextOptions {
  v8::Local<v8::String> name;
  v8::Local<v8::String> origin;
  v8::Local<v8::Boolean> allow_code_gen_strings;
  v8::Local<v8::Boolean> allow_code_gen_wasm;
  BaseObjectPtr<MicrotaskQueueWrap> microtask_queue_wrap;
};

class ContextifyContext : public BaseObject {
 public:
  ContextifyContext(Environment* env,
                    v8::Local<v8::Object> wrapper,
                    v8::Local<v8::Context> v8_context,
                    const ContextOptions& options);
  ~ContextifyContext();

  void MemoryInfo(MemoryTracker* tracker) const override;
  SET_MEMORY_INFO_NAME(ContextifyContext)
  SET_SELF_SIZE(ContextifyContext)

  static v8::MaybeLocal<v8::Context> CreateV8Context(
      v8::Isolate* isolate,
      v8::Local<v8::ObjectTemplate> object_template,
      const SnapshotData* snapshot_data,
      v8::MicrotaskQueue* queue);
  static void CreatePerIsolateProperties(IsolateData* isolate_data,
                                         v8::Local<v8::ObjectTemplate> target);
  static void RegisterExternalReferences(ExternalReferenceRegistry* registry);

  static ContextifyContext* ContextFromContextifiedSandbox(
      Environment* env,
      const v8::Local<v8::Object>& sandbox);

  inline v8::Local<v8::Context> context() const {
    return PersistentToLocal::Default(env()->isolate(), context_);
  }

  inline v8::Local<v8::Object> global_proxy() const {
    return context()->Global();
  }

  inline v8::Local<v8::Object> sandbox() const {
    return context()->GetEmbedderData(ContextEmbedderIndex::kSandboxObject)
        .As<v8::Object>();
  }

  inline std::shared_ptr<v8::MicrotaskQueue> microtask_queue() const {
    if (!microtask_queue_wrap_) return {};
    return microtask_queue_wrap_->microtask_queue();
  }

  template <typename T>
  static ContextifyContext* Get(const v8::PropertyCallbackInfo<T>& args);
  static ContextifyContext* Get(v8::Local<v8::Object> object);

  static void InitializeGlobalTemplates(IsolateData* isolate_data);

 private:
  static BaseObjectPtr<ContextifyContext> New(Environment* env,
                                              v8::Local<v8::Object> sandbox_obj,
                                              const ContextOptions& options);
  // Initialize a context created from CreateV8Context()
  static BaseObjectPtr<ContextifyContext> New(v8::Local<v8::Context> ctx,
                                              Environment* env,
                                              v8::Local<v8::Object> sandbox_obj,
                                              const ContextOptions& options);

  static bool IsStillInitializing(const ContextifyContext* ctx);
  static void MakeContext(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void IsContext(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void CompileFunction(
      const v8::FunctionCallbackInfo<v8::Value>& args);
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

  v8::Global<v8::Context> context_;
  BaseObjectPtr<MicrotaskQueueWrap> microtask_queue_wrap_;
};

class ContextifyScript : public BaseObject {
 public:
  SET_NO_MEMORY_INFO()
  SET_MEMORY_INFO_NAME(ContextifyScript)
  SET_SELF_SIZE(ContextifyScript)

  ContextifyScript(Environment* env, v8::Local<v8::Object> object);
  ~ContextifyScript() override;

  static void CreatePerIsolateProperties(IsolateData* isolate_data,
                                         v8::Local<v8::ObjectTemplate> target);
  static void RegisterExternalReferences(ExternalReferenceRegistry* registry);
  static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
  static bool InstanceOf(Environment* env, const v8::Local<v8::Value>& args);
  static void CreateCachedData(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void RunInContext(const v8::FunctionCallbackInfo<v8::Value>& args);
  static bool EvalMachine(v8::Local<v8::Context> context,
                          Environment* env,
                          const int64_t timeout,
                          const bool display_errors,
                          const bool break_on_sigint,
                          const bool break_on_first_line,
                          std::shared_ptr<v8::MicrotaskQueue> microtask_queue,
                          const v8::FunctionCallbackInfo<v8::Value>& args);

  inline uint32_t id() { return id_; }

 private:
  v8::Global<v8::UnboundScript> script_;
  uint32_t id_;
};

class CompiledFnEntry final : public BaseObject {
 public:
  SET_NO_MEMORY_INFO()
  SET_MEMORY_INFO_NAME(CompiledFnEntry)
  SET_SELF_SIZE(CompiledFnEntry)

  CompiledFnEntry(Environment* env,
                  v8::Local<v8::Object> object,
                  uint32_t id,
                  v8::Local<v8::Function> fn);
  ~CompiledFnEntry();

  bool IsNotIndicativeOfMemoryLeakAtExit() const override { return true; }

 private:
  uint32_t id_;
  v8::Global<v8::Function> fn_;

  static void WeakCallback(const v8::WeakCallbackInfo<CompiledFnEntry>& data);
};

v8::Maybe<bool> StoreCodeCacheResult(
    Environment* env,
    v8::Local<v8::Object> target,
    v8::ScriptCompiler::CompileOptions compile_options,
    const v8::ScriptCompiler::Source& source,
    bool produce_cached_data,
    std::unique_ptr<v8::ScriptCompiler::CachedData> new_cached_data);

class NodeRealm final : public node::MemoryRetainer {
 public:
  static v8::Local<v8::FunctionTemplate> GetConstructorTemplate(
      IsolateData* isolate_data);
  static void CreatePerIsolateProperties(IsolateData* isolate_data,
                                         v8::Local<v8::ObjectTemplate> target);
  static void RegisterExternalReferences(
      node::ExternalReferenceRegistry* registry);

  NodeRealm(node::Environment* env, v8::Local<v8::Object> obj);

  static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void Start(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void Load(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void SignalStop(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void Stop(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void InternalRequire(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void TryCloseAllHandles(
      const v8::FunctionCallbackInfo<v8::Value>& args);

  struct NodeRealmScope : public v8::EscapableHandleScope,
                          public v8::Context::Scope,
                          public v8::Isolate::SafeForTerminationScope {
   public:
    explicit NodeRealmScope(NodeRealm* w);
    ~NodeRealmScope();

   private:
    NodeRealm* w_;
    bool orig_can_be_terminated_;
  };

  v8::Local<v8::Context> context() const;

  void MemoryInfo(node::MemoryTracker* tracker) const override;
  SET_MEMORY_INFO_NAME(NodeRealm)
  SET_SELF_SIZE(NodeRealm)

 private:
  static NodeRealm* Unwrap(const v8::FunctionCallbackInfo<v8::Value>& arg);
  static void CleanupHook(void* arg);
  void OnExit(int code);

  void Start();
  v8::MaybeLocal<v8::Value> Load(v8::Local<v8::Function> callback);
  v8::MaybeLocal<v8::Value> RunInCallbackScope(
      v8::Local<v8::Function> callback);
  void RunLoop(uv_run_mode mode);
  void SignalStop();
  void Stop(bool may_throw);

  v8::Isolate* isolate_;
  v8::Global<v8::Object> wrap_;

  std::unique_ptr<v8::MicrotaskQueue> microtask_queue_;
  v8::Global<v8::Context> outer_context_;
  v8::Global<v8::Context> context_;
  node::IsolateData* isolate_data_ = nullptr;
  node::Environment* env_ = nullptr;
  bool signaled_stop_ = false;
  bool can_be_terminated_ = false;
};

}  // namespace contextify
}  // namespace node

#endif  // defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS

#endif  // SRC_NODE_CONTEXTIFY_H_
