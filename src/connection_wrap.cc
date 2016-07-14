#include "connection_wrap.h"

#include "env-inl.h"
#include "env.h"
#include "pipe_wrap.h"
#include "tcp_wrap.h"
#include "util.h"
#include "util-inl.h"

namespace node {

using v8::Context;
using v8::HandleScope;
using v8::Integer;
using v8::Local;
using v8::Object;
using v8::Value;


template <typename WrapType, typename UVType>
void ConnectionWrap<WrapType, UVType>::OnConnection(uv_stream_t* handle,
                                                    int status) {
  WrapType* wrap_data = static_cast<WrapType*>(handle->data);
  CHECK_NE(wrap_data, nullptr);
  CHECK_EQ(&wrap_data->handle_, reinterpret_cast<UVType*>(handle));

  Environment* env = wrap_data->env();
  HandleScope handle_scope(env->isolate());
  Context::Scope context_scope(env->context());

  // We should not be getting this callback if someone has already called
  // uv_close() on the handle.
  CHECK_EQ(wrap_data->persistent().IsEmpty(), false);

  Local<Value> argv[] = {
    Integer::New(env->isolate(), status),
    Undefined(env->isolate())
  };

  if (status == 0) {
    // Instantiate the client javascript object and handle.
    Local<Object> client_obj = WrapType::Instantiate(env, wrap_data);

    // Unwrap the client javascript object.
    WrapType* wrap;
    ASSIGN_OR_RETURN_UNWRAP(&wrap, client_obj);
    uv_stream_t* client_handle =
        reinterpret_cast<uv_stream_t*>(&wrap->handle_);
    // uv_accept can fail if the new connection has already been closed, in
    // which case an EAGAIN (resource temporarily unavailabile) will be
    // returned.
    if (uv_accept(handle, client_handle))
      return;

    // Successful accept. Call the onconnection callback in JavaScript land.
    argv[1] = client_obj;
  }
  wrap_data->MakeCallback(env->onconnection_string(), arraysize(argv), argv);
}

template void ConnectionWrap<PipeWrap, uv_pipe_t>::OnConnection(
    uv_stream_t* handle, int status);

template void ConnectionWrap<TCPWrap, uv_tcp_t>::OnConnection(
    uv_stream_t* handle, int status);


}  // namespace node
