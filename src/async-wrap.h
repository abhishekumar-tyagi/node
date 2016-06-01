#ifndef SRC_ASYNC_WRAP_H_
#define SRC_ASYNC_WRAP_H_

#if defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS

#include "base-object.h"
#include "v8.h"

#include <stdint.h>

namespace node {

#define NODE_ASYNC_ID_OFFSET 0xA1C

#define NODE_ASYNC_PROVIDER_TYPES(V)                                          \
  V(NONE)                                                                     \
  V(CRYPTO)                                                                   \
  V(FSEVENTWRAP)                                                              \
  V(FSREQWRAP)                                                                \
  V(GETADDRINFOREQWRAP)                                                       \
  V(GETNAMEINFOREQWRAP)                                                       \
  V(HTTPPARSER)                                                               \
  V(JSSTREAM)                                                                 \
  V(PIPEWRAP)                                                                 \
  V(PIPECONNECTWRAP)                                                          \
  V(PROCESSWRAP)                                                              \
  V(QUERYWRAP)                                                                \
  V(SHUTDOWNWRAP)                                                             \
  V(SIGNALWRAP)                                                               \
  V(STATWATCHER)                                                              \
  V(TCPWRAP)                                                                  \
  V(TCPCONNECTWRAP)                                                           \
  V(TIMERWRAP)                                                                \
  V(TLSWRAP)                                                                  \
  V(TTYWRAP)                                                                  \
  V(UDPWRAP)                                                                  \
  V(UDPSENDWRAP)                                                              \
  V(WRITEWRAP)                                                                \
  V(ZLIB)                                                                     \
  V(NEXTTICK)

class Environment;

class AsyncWrap : public BaseObject {
 public:
  enum ProviderType {
#define V(PROVIDER)                                                           \
    PROVIDER_ ## PROVIDER,
    NODE_ASYNC_PROVIDER_TYPES(V)
#undef V
  };

  static bool FireAsyncInitCallbacks(Environment* env,int64_t uid,v8::Local<v8::Object> object,AsyncWrap::ProviderType provider,AsyncWrap* parent);
  static void FireAsyncPreCallbacks(Environment* env, bool ranInitCallback, v8::Local<v8::Number> uid, v8::Local<v8::Object> obj);
  static void FireAsyncPostCallbacks(Environment* env, bool ranInitCallback, v8::Local<v8::Number> uid, v8::Local<v8::Object> obj, v8::Local<v8::Boolean> didUserCodeThrow);
  static void FireAsyncDestroyCallbacks(Environment* env, bool ranInitCallbacks, v8::Local<v8::Number> uid);

  inline AsyncWrap(Environment* env,
                   v8::Local<v8::Object> object,
                   ProviderType provider,
                   AsyncWrap* parent = nullptr);

  inline virtual ~AsyncWrap();

  inline ProviderType provider_type() const;

  inline int64_t get_uid() const;

  // Only call these within a valid HandleScope.
  v8::Local<v8::Value> MakeCallback(const v8::Local<v8::Function> cb,
                                     int argc,
                                     v8::Local<v8::Value>* argv);
  inline v8::Local<v8::Value> MakeCallback(const v8::Local<v8::String> symbol,
                                            int argc,
                                            v8::Local<v8::Value>* argv);
  inline v8::Local<v8::Value> MakeCallback(uint32_t index,
                                            int argc,
                                            v8::Local<v8::Value>* argv);

  virtual size_t self_size() const = 0;

  inline bool ran_init_callback() const;

 private:
  inline AsyncWrap();


  // When the async hooks init JS function is called from the constructor it is
  // expected the context object will receive a _asyncQueue object property
  // that will be used to call pre/post in MakeCallback.
  uint32_t bits_;
  const int64_t uid_;
};

void LoadAsyncWrapperInfo(Environment* env);

}  // namespace node

#endif  // defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS

#endif  // SRC_ASYNC_WRAP_H_
