#include "node_sea.h"

#include "env-inl.h"
#include "node_external_reference.h"
#include "node_internals.h"
#include "node_union_bytes.h"
#include "simdutf.h"
#include "v8.h"

#define POSTJECT_SENTINEL_FUSE "NODE_JS_FUSE_fce680ab2cc467b6e072b8b5df1996b2"
#include "postject-api.h"

#include <memory>
#include <tuple>
#include <vector>

#if !defined(DISABLE_SINGLE_EXECUTABLE_APPLICATION)

using v8::Context;
using v8::FunctionCallbackInfo;
using v8::Local;
using v8::Object;
using v8::String;
using v8::Value;

namespace {

bool single_executable_application_code_loaded = false;
size_t single_executable_application_size = 0;
const char* single_executable_application_code = nullptr;

const char* FindSingleExecutableCode(size_t* size) {
  if (single_executable_application_code_loaded == false) {
#ifdef __APPLE__
    postject_options options;
    postject_options_init(&options);
    options.macho_segment_name = "NODE_JS";
    single_executable_application_code =
        static_cast<const char*>(postject_find_resource(
            "NODE_JS_CODE", &single_executable_application_size, &options));
#else
    single_executable_application_code =
        static_cast<const char*>(postject_find_resource(
            "NODE_JS_CODE", &single_executable_application_size, nullptr));
#endif
    single_executable_application_code_loaded = true;
  }

  if (size != nullptr) {
    *size = single_executable_application_size;
  }

  return single_executable_application_code;
}

void GetSingleExecutableCode(const FunctionCallbackInfo<Value>& args) {
  node::Environment* env = node::Environment::GetCurrent(args);

  size_t size = 0;
  const char* code = FindSingleExecutableCode(&size);

  if (code == nullptr) {
    return;
  }

  size_t expected_u16_length = simdutf::utf16_length_from_utf8(code, size);
  auto out = std::make_shared<std::vector<uint16_t>>(expected_u16_length);
  size_t u16_length = simdutf::convert_utf8_to_utf16(
      code, size, reinterpret_cast<char16_t*>(out->data()));
  out->resize(u16_length);

  args.GetReturnValue().Set(
      node::UnionBytes(out).ToStringChecked(env->isolate()));
}

}  // namespace

namespace node {
namespace per_process {
namespace sea {

bool IsSingleExecutable() {
  return postject_has_resource();
}

std::tuple<int, char**> FixupArgsForSEA(int argc, char** argv) {
  // Repeats argv[0] at position 1 on argv as a replacement for the missing
  // entry point file path.
  if (IsSingleExecutable()) {
    char** new_argv = new char*[argc + 2];
    int new_argc = 0;
    new_argv[new_argc++] = argv[0];
    new_argv[new_argc++] = argv[0];

    for (int i = 1; i < argc; ++i) {
      new_argv[new_argc++] = argv[i];
    }

    new_argv[new_argc] = nullptr;

    argc = new_argc;
    argv = new_argv;
  }

  return {argc, argv};
}

void Initialize(Local<Object> target,
                Local<Value> unused,
                Local<Context> context,
                void* priv) {
  SetMethod(
      context, target, "getSingleExecutableCode", GetSingleExecutableCode);
}

void RegisterExternalReferences(ExternalReferenceRegistry* registry) {
  registry->Register(GetSingleExecutableCode);
}

}  // namespace sea
}  // namespace per_process
}  // namespace node

NODE_BINDING_CONTEXT_AWARE_INTERNAL(sea, node::per_process::sea::Initialize)
NODE_BINDING_EXTERNAL_REFERENCE(
    sea, node::per_process::sea::RegisterExternalReferences)

#endif  // !defined(DISABLE_SINGLE_EXECUTABLE_APPLICATION)
