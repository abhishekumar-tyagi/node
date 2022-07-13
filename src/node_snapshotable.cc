
#include "node_snapshotable.h"
#include <iostream>
#include <sstream>
#include "base_object-inl.h"
#include "debug_utils-inl.h"
#include "env-inl.h"
#include "node_blob.h"
#include "node_errors.h"
#include "node_external_reference.h"
#include "node_file.h"
#include "node_internals.h"
#include "node_main_instance.h"
#include "node_native_module_env.h"
#include "node_process.h"
#include "node_snapshot_builder.h"
#include "node_v8.h"
#include "node_v8_platform-inl.h"

#if HAVE_INSPECTOR
#include "inspector/worker_inspector.h"  // ParentInspectorHandle
#endif

namespace node {

using v8::Context;
using v8::Function;
using v8::FunctionCallbackInfo;
using v8::HandleScope;
using v8::Isolate;
using v8::Local;
using v8::Object;
using v8::ScriptCompiler;
using v8::ScriptOrigin;
using v8::SnapshotCreator;
using v8::StartupData;
using v8::String;
using v8::TryCatch;
using v8::Value;

std::unique_ptr<SnapshotData> SnapshotData::New() {
  std::unique_ptr<SnapshotData> result = std::make_unique<SnapshotData>();
  result->data_ownership = DataOwnership::kOwned;
  result->v8_snapshot_blob_data.data = nullptr;
  return result;
}

SnapshotData::~SnapshotData() {
  if (data_ownership == DataOwnership::kOwned &&
      v8_snapshot_blob_data.data != nullptr) {
    delete[] v8_snapshot_blob_data.data;
  }
}

template <typename T>
void WriteVector(std::ostream* ss, const T* vec, size_t size) {
  for (size_t i = 0; i < size; i++) {
    *ss << std::to_string(vec[i]) << (i == size - 1 ? '\n' : ',');
  }
}

static std::string GetCodeCacheDefName(const std::string& id) {
  char buf[64] = {0};
  size_t size = id.size();
  CHECK_LT(size, sizeof(buf));
  for (size_t i = 0; i < size; ++i) {
    char ch = id[i];
    buf[i] = (ch == '-' || ch == '/') ? '_' : ch;
  }
  return std::string(buf) + std::string("_cache_data");
}

static std::string FormatSize(size_t size) {
  char buf[64] = {0};
  if (size < 1024) {
    snprintf(buf, sizeof(buf), "%.2fB", static_cast<double>(size));
  } else if (size < 1024 * 1024) {
    snprintf(buf, sizeof(buf), "%.2fKB", static_cast<double>(size / 1024));
  } else {
    snprintf(
        buf, sizeof(buf), "%.2fMB", static_cast<double>(size / 1024 / 1024));
  }
  return buf;
}

static void WriteStaticCodeCacheData(std::ostream* ss,
                                     const native_module::CodeCacheInfo& info) {
  *ss << "static const uint8_t " << GetCodeCacheDefName(info.id) << "[] = {\n";
  WriteVector(ss, info.data.data(), info.data.size());
  *ss << "};";
}

static void WriteCodeCacheInitializer(std::ostream* ss, const std::string& id) {
  std::string def_name = GetCodeCacheDefName(id);
  *ss << "    { \"" << id << "\",\n";
  *ss << "      {" << def_name << ",\n";
  *ss << "       " << def_name << " + arraysize(" << def_name << "),\n";
  *ss << "      }\n";
  *ss << "    },\n";
}

void FormatBlob(std::ostream& ss, SnapshotData* data) {
  ss << R"(#include <cstddef>
#include "env.h"
#include "node_snapshot_builder.h"
#include "v8.h"

// This file is generated by tools/snapshot. Do not edit.

namespace node {

static const char v8_snapshot_blob_data[] = {
)";
  WriteVector(&ss,
              data->v8_snapshot_blob_data.data,
              data->v8_snapshot_blob_data.raw_size);
  ss << R"(};

static const int v8_snapshot_blob_size = )"
     << data->v8_snapshot_blob_data.raw_size << ";";

  // Windows can't deal with too many large vector initializers.
  // Store the data into static arrays first.
  for (const auto& item : data->code_cache) {
    WriteStaticCodeCacheData(&ss, item);
  }

  ss << R"(SnapshotData snapshot_data {
  // -- data_ownership begins --
  SnapshotData::DataOwnership::kNotOwned,
  // -- data_ownership ends --
  // -- v8_snapshot_blob_data begins --
  { v8_snapshot_blob_data, v8_snapshot_blob_size },
  // -- v8_snapshot_blob_data ends --
  // -- isolate_data_indices begins --
  {
)";
  WriteVector(&ss,
              data->isolate_data_indices.data(),
              data->isolate_data_indices.size());
  ss << R"(},
  // -- isolate_data_indices ends --
  // -- env_info begins --
)" << data->env_info
     << R"(
  // -- env_info ends --
  ,
  // -- code_cache begins --
  {)";
  for (const auto& item : data->code_cache) {
    WriteCodeCacheInitializer(&ss, item.id);
  }
  ss << R"(
  }
  // -- code_cache ends --
};

const SnapshotData* SnapshotBuilder::GetEmbeddedSnapshotData() {
  Mutex::ScopedLock lock(snapshot_data_mutex_);
  return &snapshot_data;
}
}  // namespace node
)";
}

Mutex SnapshotBuilder::snapshot_data_mutex_;

const std::vector<intptr_t>& SnapshotBuilder::CollectExternalReferences() {
  static auto registry = std::make_unique<ExternalReferenceRegistry>();
  return registry->external_references();
}

void SnapshotBuilder::InitializeIsolateParams(const SnapshotData* data,
                                              Isolate::CreateParams* params) {
  params->external_references = CollectExternalReferences().data();
  params->snapshot_blob =
      const_cast<v8::StartupData*>(&(data->v8_snapshot_blob_data));
}

// TODO(joyeecheung): share these exit code constants across the code base.
constexpr int UNCAUGHT_EXCEPTION_ERROR = 1;
constexpr int BOOTSTRAP_ERROR = 10;
constexpr int SNAPSHOT_ERROR = 14;

int SnapshotBuilder::Generate(SnapshotData* out,
                              const std::vector<std::string> args,
                              const std::vector<std::string> exec_args) {
  const std::vector<intptr_t>& external_references =
      CollectExternalReferences();
  Isolate* isolate = Isolate::Allocate();
  // Must be done before the SnapshotCreator creation so  that the
  // memory reducer can be initialized.
  per_process::v8_platform.Platform()->RegisterIsolate(isolate,
                                                       uv_default_loop());

  SnapshotCreator creator(isolate, external_references.data());

  isolate->SetCaptureStackTraceForUncaughtExceptions(
      true, 10, v8::StackTrace::StackTraceOptions::kDetailed);

  Environment* env = nullptr;
  std::unique_ptr<NodeMainInstance> main_instance =
      NodeMainInstance::Create(isolate,
                               uv_default_loop(),
                               per_process::v8_platform.Platform(),
                               args,
                               exec_args);

  // The cleanups should be done in case of an early exit due to errors.
  auto cleanup = OnScopeLeave([&]() {
    // Must be done while the snapshot creator isolate is entered i.e. the
    // creator is still alive. The snapshot creator destructor will destroy
    // the isolate.
    if (env != nullptr) {
      FreeEnvironment(env);
    }
    main_instance->Dispose();
    per_process::v8_platform.Platform()->UnregisterIsolate(isolate);
  });

  {
    HandleScope scope(isolate);
    TryCatch bootstrapCatch(isolate);

    auto print_Exception = OnScopeLeave([&]() {
      if (bootstrapCatch.HasCaught()) {
        PrintCaughtException(
            isolate, isolate->GetCurrentContext(), bootstrapCatch);
      }
    });

    out->isolate_data_indices =
        main_instance->isolate_data()->Serialize(&creator);

    // The default context with only things created by V8.
    Local<Context> default_context = Context::New(isolate);

    // The Node.js-specific context with primodials, can be used by workers
    // TODO(joyeecheung): investigate if this can be used by vm contexts
    // without breaking compatibility.
    Local<Context> base_context = NewContext(isolate);
    if (base_context.IsEmpty()) {
      return BOOTSTRAP_ERROR;
    }

    Local<Context> main_context = NewContext(isolate);
    if (main_context.IsEmpty()) {
      return BOOTSTRAP_ERROR;
    }
    // Initialize the main instance context.
    {
      Context::Scope context_scope(main_context);

      // Create the environment.
      env = new Environment(main_instance->isolate_data(),
                            main_context,
                            args,
                            exec_args,
                            nullptr,
                            node::EnvironmentFlags::kDefaultFlags,
                            {});

      // Run scripts in lib/internal/bootstrap/
      if (env->RunBootstrapping().IsEmpty()) {
        return BOOTSTRAP_ERROR;
      }
      // If --build-snapshot is true, lib/internal/main/mksnapshot.js would be
      // loaded via LoadEnvironment() to execute process.argv[1] as the entry
      // point (we currently only support this kind of entry point, but we
      // could also explore snapshotting other kinds of execution modes
      // in the future).
      if (per_process::cli_options->build_snapshot) {
#if HAVE_INSPECTOR
        // TODO(joyeecheung): move this before RunBootstrapping().
        env->InitializeInspector({});
#endif
        if (LoadEnvironment(env, StartExecutionCallback{}).IsEmpty()) {
          return UNCAUGHT_EXCEPTION_ERROR;
        }
        // FIXME(joyeecheung): right now running the loop in the snapshot
        // builder seems to introduces inconsistencies in JS land that need to
        // be synchronized again after snapshot restoration.
        int exit_code = SpinEventLoop(env).FromMaybe(UNCAUGHT_EXCEPTION_ERROR);
        if (exit_code != 0) {
          return exit_code;
        }
      }

      if (per_process::enabled_debug_list.enabled(DebugCategory::MKSNAPSHOT)) {
        env->PrintAllBaseObjects();
        printf("Environment = %p\n", env);
      }

      // Serialize the native states
      out->env_info = env->Serialize(&creator);

#ifdef NODE_USE_NODE_CODE_CACHE
      // Regenerate all the code cache.
      if (!native_module::NativeModuleEnv::CompileAllModules(main_context)) {
        return UNCAUGHT_EXCEPTION_ERROR;
      }
      native_module::NativeModuleEnv::CopyCodeCache(&(out->code_cache));
      for (const auto& item : out->code_cache) {
        std::string size_str = FormatSize(item.data.size());
        per_process::Debug(DebugCategory::MKSNAPSHOT,
                           "Generated code cache for %d: %s\n",
                           item.id.c_str(),
                           size_str.c_str());
      }
#endif
    }

    // Global handles to the contexts can't be disposed before the
    // blob is created. So initialize all the contexts before adding them.
    // TODO(joyeecheung): figure out how to remove this restriction.
    creator.SetDefaultContext(default_context);
    size_t index = creator.AddContext(base_context);
    CHECK_EQ(index, SnapshotData::kNodeBaseContextIndex);
    index = creator.AddContext(main_context,
                               {SerializeNodeContextInternalFields, env});
    CHECK_EQ(index, SnapshotData::kNodeMainContextIndex);
  }

  // Must be out of HandleScope
  out->v8_snapshot_blob_data =
      creator.CreateBlob(SnapshotCreator::FunctionCodeHandling::kClear);

  // We must be able to rehash the blob when we restore it or otherwise
  // the hash seed would be fixed by V8, introducing a vulnerability.
  if (!out->v8_snapshot_blob_data.CanBeRehashed()) {
    return SNAPSHOT_ERROR;
  }

  // We cannot resurrect the handles from the snapshot, so make sure that
  // no handles are left open in the environment after the blob is created
  // (which should trigger a GC and close all handles that can be closed).
  bool queues_are_empty =
      env->req_wrap_queue()->IsEmpty() && env->handle_wrap_queue()->IsEmpty();
  if (!queues_are_empty ||
      per_process::enabled_debug_list.enabled(DebugCategory::MKSNAPSHOT)) {
    PrintLibuvHandleInformation(env->event_loop(), stderr);
  }
  if (!queues_are_empty) {
    return SNAPSHOT_ERROR;
  }
  return 0;
}

int SnapshotBuilder::Generate(std::ostream& out,
                              const std::vector<std::string> args,
                              const std::vector<std::string> exec_args) {
  std::unique_ptr<SnapshotData> data = SnapshotData::New();
  int exit_code = Generate(data.get(), args, exec_args);
  if (exit_code != 0) {
    return exit_code;
  }
  FormatBlob(out, data.get());
  return exit_code;
}

SnapshotableObject::SnapshotableObject(Environment* env,
                                       Local<Object> wrap,
                                       EmbedderObjectType type)
    : BaseObject(env, wrap), type_(type) {
}

const char* SnapshotableObject::GetTypeNameChars() const {
  switch (type_) {
#define V(PropertyName, NativeTypeName)                                        \
  case EmbedderObjectType::k_##PropertyName: {                                 \
    return NativeTypeName::type_name.c_str();                                  \
  }
    SERIALIZABLE_OBJECT_TYPES(V)
#undef V
    default: { UNREACHABLE(); }
  }
}

bool IsSnapshotableType(FastStringKey key) {
#define V(PropertyName, NativeTypeName)                                        \
  if (key == NativeTypeName::type_name) {                                      \
    return true;                                                               \
  }
  SERIALIZABLE_OBJECT_TYPES(V)
#undef V

  return false;
}

void DeserializeNodeInternalFields(Local<Object> holder,
                                   int index,
                                   StartupData payload,
                                   void* env) {
  per_process::Debug(DebugCategory::MKSNAPSHOT,
                     "Deserialize internal field %d of %p, size=%d\n",
                     static_cast<int>(index),
                     (*holder),
                     static_cast<int>(payload.raw_size));
  if (payload.raw_size == 0) {
    holder->SetAlignedPointerInInternalField(index, nullptr);
    return;
  }

  Environment* env_ptr = static_cast<Environment*>(env);
  const InternalFieldInfo* info =
      reinterpret_cast<const InternalFieldInfo*>(payload.data);

  switch (info->type) {
#define V(PropertyName, NativeTypeName)                                        \
  case EmbedderObjectType::k_##PropertyName: {                                 \
    per_process::Debug(DebugCategory::MKSNAPSHOT,                              \
                       "Object %p is %s\n",                                    \
                       (*holder),                                              \
                       NativeTypeName::type_name.c_str());                     \
    env_ptr->EnqueueDeserializeRequest(                                        \
        NativeTypeName::Deserialize, holder, index, info->Copy());             \
    break;                                                                     \
  }
    SERIALIZABLE_OBJECT_TYPES(V)
#undef V
    default: { UNREACHABLE(); }
  }
}

StartupData SerializeNodeContextInternalFields(Local<Object> holder,
                                               int index,
                                               void* env) {
  per_process::Debug(DebugCategory::MKSNAPSHOT,
                     "Serialize internal field, index=%d, holder=%p\n",
                     static_cast<int>(index),
                     *holder);
  void* ptr = holder->GetAlignedPointerFromInternalField(BaseObject::kSlot);
  if (ptr == nullptr) {
    return StartupData{nullptr, 0};
  }

  DCHECK(static_cast<BaseObject*>(ptr)->is_snapshotable());
  SnapshotableObject* obj = static_cast<SnapshotableObject*>(ptr);
  per_process::Debug(DebugCategory::MKSNAPSHOT,
                     "Object %p is %s, ",
                     *holder,
                     obj->GetTypeNameChars());
  InternalFieldInfo* info = obj->Serialize(index);
  per_process::Debug(DebugCategory::MKSNAPSHOT,
                     "payload size=%d\n",
                     static_cast<int>(info->length));
  return StartupData{reinterpret_cast<const char*>(info),
                     static_cast<int>(info->length)};
}

void SerializeBindingData(Environment* env,
                          SnapshotCreator* creator,
                          EnvSerializeInfo* info) {
  size_t i = 0;
  env->ForEachBindingData([&](FastStringKey key,
                              BaseObjectPtr<BaseObject> binding) {
    per_process::Debug(DebugCategory::MKSNAPSHOT,
                       "Serialize binding %i, %p, type=%s\n",
                       static_cast<int>(i),
                       *(binding->object()),
                       key.c_str());

    if (IsSnapshotableType(key)) {
      size_t index = creator->AddData(env->context(), binding->object());
      per_process::Debug(DebugCategory::MKSNAPSHOT,
                         "Serialized with index=%d\n",
                         static_cast<int>(index));
      info->bindings.push_back({key.c_str(), i, index});
      SnapshotableObject* ptr = static_cast<SnapshotableObject*>(binding.get());
      ptr->PrepareForSerialization(env->context(), creator);
    } else {
      UNREACHABLE();
    }

    i++;
  });
}

namespace mksnapshot {

void CompileSerializeMain(const FunctionCallbackInfo<Value>& args) {
  CHECK(args[0]->IsString());
  Local<String> filename = args[0].As<String>();
  Local<String> source = args[1].As<String>();
  Isolate* isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  ScriptOrigin origin(isolate, filename, 0, 0, true);
  // TODO(joyeecheung): do we need all of these? Maybe we would want a less
  // internal version of them.
  std::vector<Local<String>> parameters = {
      FIXED_ONE_BYTE_STRING(isolate, "require"),
      FIXED_ONE_BYTE_STRING(isolate, "__filename"),
      FIXED_ONE_BYTE_STRING(isolate, "__dirname"),
  };
  ScriptCompiler::Source script_source(source, origin);
  Local<Function> fn;
  if (ScriptCompiler::CompileFunctionInContext(context,
                                               &script_source,
                                               parameters.size(),
                                               parameters.data(),
                                               0,
                                               nullptr,
                                               ScriptCompiler::kEagerCompile)
          .ToLocal(&fn)) {
    args.GetReturnValue().Set(fn);
  }
}

void SetSerializeCallback(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  CHECK(env->snapshot_serialize_callback().IsEmpty());
  CHECK(args[0]->IsFunction());
  env->set_snapshot_serialize_callback(args[0].As<Function>());
}

void SetDeserializeCallback(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  CHECK(env->snapshot_deserialize_callback().IsEmpty());
  CHECK(args[0]->IsFunction());
  env->set_snapshot_deserialize_callback(args[0].As<Function>());
}

void SetDeserializeMainFunction(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  CHECK(env->snapshot_deserialize_main().IsEmpty());
  CHECK(args[0]->IsFunction());
  env->set_snapshot_deserialize_main(args[0].As<Function>());
}

void Initialize(Local<Object> target,
                Local<Value> unused,
                Local<Context> context,
                void* priv) {
  Environment* env = Environment::GetCurrent(context);
  env->SetMethod(target, "compileSerializeMain", CompileSerializeMain);
  env->SetMethod(target, "markBootstrapComplete", MarkBootstrapComplete);
  env->SetMethod(target, "setSerializeCallback", SetSerializeCallback);
  env->SetMethod(target, "setDeserializeCallback", SetDeserializeCallback);
  env->SetMethod(
      target, "setDeserializeMainFunction", SetDeserializeMainFunction);
}

void RegisterExternalReferences(ExternalReferenceRegistry* registry) {
  registry->Register(CompileSerializeMain);
  registry->Register(MarkBootstrapComplete);
  registry->Register(SetSerializeCallback);
  registry->Register(SetDeserializeCallback);
  registry->Register(SetDeserializeMainFunction);
}
}  // namespace mksnapshot
}  // namespace node

NODE_MODULE_CONTEXT_AWARE_INTERNAL(mksnapshot, node::mksnapshot::Initialize)
NODE_MODULE_EXTERNAL_REFERENCE(mksnapshot,
                               node::mksnapshot::RegisterExternalReferences)
