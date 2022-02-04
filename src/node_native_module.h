#ifndef SRC_NODE_NATIVE_MODULE_H_
#define SRC_NODE_NATIVE_MODULE_H_

#if defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS

#include <map>
#include <memory>
#include <set>
#include <string>
#include "node_mutex.h"
#include "node_union_bytes.h"
#include "v8.h"

// Forward declare test fixture for `friend` declaration.
class PerProcessTest;

namespace node {
namespace native_module {

using NativeModuleRecordMap = std::map<std::string, UnionBytes>;
using NativeModuleCacheMap =
    std::unordered_map<std::string,
                       std::unique_ptr<v8::ScriptCompiler::CachedData>>;

// The native (C++) side of the NativeModule in JS land, which
// handles compilation and caching of builtin modules (NativeModule)
// and bootstrappers, whose source are bundled into the binary
// as static data.
// This class should not depend on any Environment, or depend on access to
// the its own singleton - that should be encapsulated in NativeModuleEnv
// instead.
class NODE_EXTERN_PRIVATE NativeModuleLoader {
 public:
  NativeModuleLoader(const NativeModuleLoader&) = delete;
  NativeModuleLoader& operator=(const NativeModuleLoader&) = delete;

 private:
  // Only allow access from friends.
  friend class NativeModuleEnv;
  friend class CodeCacheBuilder;

  NativeModuleLoader();
  static NativeModuleLoader* GetInstance();

  // Generated by tools/js2c.py as node_javascript.cc
  void LoadJavaScriptSource();  // Loads data into source_
  UnionBytes GetConfig();       // Return data for config.gypi

  bool Exists(const char* id);
  bool Add(const char* id, const UnionBytes& source);

  v8::Local<v8::Object> GetSourceObject(v8::Local<v8::Context> context);
  v8::Local<v8::String> GetConfigString(v8::Isolate* isolate);
  std::vector<std::string> GetModuleIds();

  struct ModuleCategories {
    bool is_initialized = false;
    std::set<std::string> can_be_required;
    std::set<std::string> cannot_be_required;
  };
  void InitializeModuleCategories();
  const std::set<std::string>& GetCannotBeRequired();
  const std::set<std::string>& GetCanBeRequired();

  bool CanBeRequired(const char* id);
  bool CannotBeRequired(const char* id);

  NativeModuleCacheMap* code_cache();
  v8::ScriptCompiler::CachedData* GetCodeCache(const char* id) const;
  enum class Result { kWithCache, kWithoutCache };
  v8::MaybeLocal<v8::String> LoadBuiltinModuleSource(v8::Isolate* isolate,
                                                     const char* id);
  // If an exception is encountered (e.g. source code contains
  // syntax error), the returned value is empty.
  v8::MaybeLocal<v8::Function> LookupAndCompile(
      v8::Local<v8::Context> context,
      const char* id,
      std::vector<v8::Local<v8::String>>* parameters,
      Result* result);
  v8::MaybeLocal<v8::Function> CompileAsModule(v8::Local<v8::Context> context,
                                               const char* id,
                                               Result* result);

  static NativeModuleLoader instance_;
  ModuleCategories module_categories_;
  NativeModuleRecordMap source_;
  NativeModuleCacheMap code_cache_;
  UnionBytes config_;

  // Used to synchronize access to the code cache map
  Mutex code_cache_mutex_;

  friend class ::PerProcessTest;
};
}  // namespace native_module

}  // namespace node

#endif  // defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS

#endif  // SRC_NODE_NATIVE_MODULE_H_
