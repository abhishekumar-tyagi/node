#ifndef SRC_CONNECTION_WRAP_H_
#define SRC_CONNECTION_WRAP_H_

#include "stream_wrap.h"

namespace node {

template <typename WrapType, typename UVType>
class ConnectionWrap : public StreamWrap {
 public:
  UVType* UVHandle() {
    return &handle_;
  }

  static void OnConnection(uv_stream_t* handle, int status);
  static void AfterConnect(uv_connect_t* req, int status);

 protected:
  ConnectionWrap(Environment* env,
                 v8::Local<v8::Object> object,
                 ProviderType provider,
                 AsyncWrap* parent);
  ~ConnectionWrap() {
  }

  UVType handle_;
};

}  // namespace node

#endif  // SRC_CONNECTION_WRAP_H_
