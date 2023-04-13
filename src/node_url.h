#ifndef SRC_NODE_URL_H_
#define SRC_NODE_URL_H_

#if defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS

#include <cinttypes>
#include "ada.h"
#include "aliased_buffer.h"
#include "node.h"
#include "node_snapshotable.h"
#include "util.h"
#include "v8.h"
#include "v8-fast-api-calls.h"

#include <string>

namespace node {
class ExternalReferenceRegistry;

namespace url {

enum url_update_action {
  kProtocol = 0,
  kHost = 1,
  kHostname = 2,
  kPort = 3,
  kUsername = 4,
  kPassword = 5,
  kPathname = 6,
  kSearch = 7,
  kHash = 8,
  kHref = 9,
};

class BindingData : public SnapshotableObject {
 public:
  BindingData(Realm* realm, v8::Local<v8::Object> obj);

  using InternalFieldInfo = InternalFieldInfoBase;

  SERIALIZABLE_OBJECT_METHODS()
  SET_BINDING_ID(url_binding_data)

  void MemoryInfo(MemoryTracker* tracker) const override;
  SET_SELF_SIZE(BindingData)
  SET_MEMORY_INFO_NAME(BindingData)

  static void DomainToASCII(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void DomainToUnicode(const v8::FunctionCallbackInfo<v8::Value>& args);

  static void CanParse(const v8::FunctionCallbackInfo<v8::Value>& args);
  static bool FastCanParse(const v8::FastOneByteString& input,
                           const v8::FastOneByteString& base);

  static void Format(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void Parse(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void Update(const v8::FunctionCallbackInfo<v8::Value>& args);

  static void Initialize(v8::Local<v8::Object> target,
                         v8::Local<v8::Value> unused,
                         v8::Local<v8::Context> context,
                         void* priv);
  static void RegisterExternalReferences(ExternalReferenceRegistry* registry);

 private:
  static constexpr size_t kURLComponentsLength = 9;
  AliasedUint32Array url_components_buffer_;

  void UpdateComponents(const ada::url_components& components,
                        const ada::scheme::type type);

  static v8::CFunction fast_canParse_;
};

std::string FromFilePath(const std::string_view file_path);

}  // namespace url

}  // namespace node

#endif  // defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS

#endif  // SRC_NODE_URL_H_
