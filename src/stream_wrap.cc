#include "stream_wrap.h"
#include "env-inl.h"
#include "env.h"
#include "handle_wrap.h"
#include "node_buffer.h"
#include "node_counters.h"
#include "pipe_wrap.h"
#include "req-wrap.h"
#include "req-wrap-inl.h"
#include "tcp_wrap.h"
#include "udp_wrap.h"
#include "util.h"
#include "util-inl.h"

#include <stdlib.h>  // abort()
#include <string.h>  // memcpy()
#include <limits.h>  // INT_MAX


namespace node {

using v8::Array;
using v8::Context;
using v8::EscapableHandleScope;
using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::Handle;
using v8::HandleScope;
using v8::Integer;
using v8::Local;
using v8::Number;
using v8::Object;
using v8::PropertyCallbackInfo;
using v8::String;
using v8::True;
using v8::Undefined;
using v8::Value;


void StreamWrap::Initialize(Handle<Object> target,
                            Handle<Value> unused,
                            Handle<Context> context) {
  Environment* env = Environment::GetCurrent(context);

  Local<FunctionTemplate> sw =
      FunctionTemplate::New(env->isolate(), ShutdownWrap::NewShutdownWrap);
  sw->InstanceTemplate()->SetInternalFieldCount(1);
  sw->SetClassName(FIXED_ONE_BYTE_STRING(env->isolate(), "ShutdownWrap"));
  target->Set(FIXED_ONE_BYTE_STRING(env->isolate(), "ShutdownWrap"),
              sw->GetFunction());

  Local<FunctionTemplate> ww =
      FunctionTemplate::New(env->isolate(), WriteWrap::NewWriteWrap);
  ww->InstanceTemplate()->SetInternalFieldCount(1);
  ww->SetClassName(FIXED_ONE_BYTE_STRING(env->isolate(), "WriteWrap"));
  target->Set(FIXED_ONE_BYTE_STRING(env->isolate(), "WriteWrap"),
              ww->GetFunction());
}


StreamWrap::StreamWrap(Environment* env,
                       Local<Object> object,
                       uv_stream_t* stream,
                       AsyncWrap::ProviderType provider,
                       AsyncWrap* parent)
    : HandleWrap(env,
                 object,
                 reinterpret_cast<uv_handle_t*>(stream),
                 provider,
                 parent),
      StreamBase(env, object),
      stream_(stream),
      default_callbacks_(this),
      callbacks_(&default_callbacks_),
      callbacks_gc_(false) {
}


int StreamWrap::GetFD() const {
  int fd = -1;
#if !defined(_WIN32)
  if (stream() != nullptr)
    fd = stream()->io_watcher.fd;
#endif
  return fd;
}


bool StreamWrap::IsAlive() const {
  return HandleWrap::IsAlive(this);
}


Local<Object> StreamWrap::GetObject() {
  return object();
}


bool StreamWrap::IsIPCPipe() const {
  return is_named_pipe_ipc();
}


void StreamWrap::UpdateWriteQueueSize() {
  HandleScope scope(env()->isolate());
  Local<Integer> write_queue_size =
      Integer::NewFromUnsigned(env()->isolate(), stream()->write_queue_size);
  object()->Set(env()->write_queue_size_string(), write_queue_size);
}


int StreamWrap::ReadStart() {
  return uv_read_start(stream(), OnAlloc, OnRead);
}


int StreamWrap::ReadStop() {
  return uv_read_stop(stream());
}


void StreamWrap::OnAlloc(uv_handle_t* handle,
                         size_t suggested_size,
                         uv_buf_t* buf) {
  StreamWrap* wrap = static_cast<StreamWrap*>(handle->data);
  CHECK_EQ(wrap->stream(), reinterpret_cast<uv_stream_t*>(handle));
  wrap->callbacks()->DoAlloc(handle, suggested_size, buf);
}


template <class WrapType, class UVType>
static Local<Object> AcceptHandle(Environment* env,
                                  uv_stream_t* pipe,
                                  AsyncWrap* parent) {
  EscapableHandleScope scope(env->isolate());
  Local<Object> wrap_obj;
  UVType* handle;

  wrap_obj = WrapType::Instantiate(env, parent);
  if (wrap_obj.IsEmpty())
    return Local<Object>();

  WrapType* wrap = Unwrap<WrapType>(wrap_obj);
  handle = wrap->UVHandle();

  if (uv_accept(pipe, reinterpret_cast<uv_stream_t*>(handle)))
    abort();

  return scope.Escape(wrap_obj);
}


void StreamWrap::OnReadCommon(uv_stream_t* handle,
                              ssize_t nread,
                              const uv_buf_t* buf,
                              uv_handle_type pending) {
  StreamWrap* wrap = static_cast<StreamWrap*>(handle->data);

  // We should not be getting this callback if someone as already called
  // uv_close() on the handle.
  CHECK_EQ(wrap->persistent().IsEmpty(), false);

  if (nread > 0) {
    if (wrap->is_tcp()) {
      NODE_COUNT_NET_BYTES_RECV(nread);
    } else if (wrap->is_named_pipe()) {
      NODE_COUNT_PIPE_BYTES_RECV(nread);
    }
  }

  wrap->callbacks()->DoRead(handle, nread, buf, pending);
}


void StreamWrap::OnRead(uv_stream_t* handle,
                        ssize_t nread,
                        const uv_buf_t* buf) {
  StreamWrap* wrap = static_cast<StreamWrap*>(handle->data);
  uv_handle_type type = UV_UNKNOWN_HANDLE;

  if (wrap->is_named_pipe_ipc() &&
      uv_pipe_pending_count(reinterpret_cast<uv_pipe_t*>(handle)) > 0) {
    type = uv_pipe_pending_type(reinterpret_cast<uv_pipe_t*>(handle));
  }

  OnReadCommon(handle, nread, buf, type);
}


int StreamWrap::SetBlocking(bool enable) {
  return uv_stream_set_blocking(stream(), enable);
}


int StreamWrap::DoShutdown(ShutdownWrap* req_wrap, uv_shutdown_cb cb) {
  return callbacks()->DoShutdown(req_wrap, cb);
}


int StreamWrap::TryWrite(uv_buf_t** bufs, size_t* count) {
  return callbacks()->TryWrite(bufs, count);
}


int StreamWrap::DoWrite(WriteWrap* w,
                        uv_buf_t* bufs,
                        size_t count,
                        uv_stream_t* send_handle,
                        uv_write_cb cb) {
  return callbacks()->DoWrite(w, bufs, count, send_handle, cb);
}


void StreamWrap::DoAfterWrite(WriteWrap* w) {
  return callbacks()->AfterWrite(w);
}


const char* StreamWrap::Error() const {
  return callbacks()->Error();
}


void StreamWrap::ClearError() {
  return callbacks()->ClearError();
}


const char* StreamWrapCallbacks::Error() const {
  return nullptr;
}


void StreamWrapCallbacks::ClearError() {
}


// NOTE: Call to this function could change both `buf`'s and `count`'s
// values, shifting their base and decrementing their length. This is
// required in order to skip the data that was successfully written via
// uv_try_write().
int StreamWrapCallbacks::TryWrite(uv_buf_t** bufs, size_t* count) {
  int err;
  size_t written;
  uv_buf_t* vbufs = *bufs;
  size_t vcount = *count;

  err = uv_try_write(wrap()->stream(), vbufs, vcount);
  if (err == UV_ENOSYS || err == UV_EAGAIN)
    return 0;
  if (err < 0)
    return err;

  // Slice off the buffers: skip all written buffers and slice the one that
  // was partially written.
  written = err;
  for (; written != 0 && vcount > 0; vbufs++, vcount--) {
    // Slice
    if (vbufs[0].len > written) {
      vbufs[0].base += written;
      vbufs[0].len -= written;
      written = 0;
      break;

    // Discard
    } else {
      written -= vbufs[0].len;
    }
  }

  *bufs = vbufs;
  *count = vcount;

  return 0;
}


int StreamWrapCallbacks::DoWrite(WriteWrap* w,
                                 uv_buf_t* bufs,
                                 size_t count,
                                 uv_stream_t* send_handle,
                                 uv_write_cb cb) {
  int r;
  if (send_handle == nullptr) {
    r = uv_write(&w->req_, wrap()->stream(), bufs, count, cb);
  } else {
    r = uv_write2(&w->req_, wrap()->stream(), bufs, count, send_handle, cb);
  }

  if (!r) {
    size_t bytes = 0;
    for (size_t i = 0; i < count; i++)
      bytes += bufs[i].len;
    if (wrap()->stream()->type == UV_TCP) {
      NODE_COUNT_NET_BYTES_SENT(bytes);
    } else if (wrap()->stream()->type == UV_NAMED_PIPE) {
      NODE_COUNT_PIPE_BYTES_SENT(bytes);
    }
  }

  wrap()->UpdateWriteQueueSize();

  return r;
}


void StreamWrapCallbacks::AfterWrite(WriteWrap* w) {
  wrap()->UpdateWriteQueueSize();
}


void StreamWrapCallbacks::DoAlloc(uv_handle_t* handle,
                                  size_t suggested_size,
                                  uv_buf_t* buf) {
  buf->base = static_cast<char*>(malloc(suggested_size));
  buf->len = suggested_size;

  if (buf->base == nullptr && suggested_size > 0) {
    FatalError(
        "node::StreamWrapCallbacks::DoAlloc(uv_handle_t*, size_t, uv_buf_t*)",
        "Out Of Memory");
  }
}


void StreamWrapCallbacks::DoRead(uv_stream_t* handle,
                                 ssize_t nread,
                                 const uv_buf_t* buf,
                                 uv_handle_type pending) {
  Environment* env = wrap()->env();
  HandleScope handle_scope(env->isolate());
  Context::Scope context_scope(env->context());

  Local<Value> argv[] = {
    Integer::New(env->isolate(), nread),
    Undefined(env->isolate()),
    Undefined(env->isolate())
  };

  if (nread < 0)  {
    if (buf->base != nullptr)
      free(buf->base);
    wrap()->MakeCallback(env->onread_string(), ARRAY_SIZE(argv), argv);
    return;
  }

  if (nread == 0) {
    if (buf->base != nullptr)
      free(buf->base);
    return;
  }

  char* base = static_cast<char*>(realloc(buf->base, nread));
  CHECK_LE(static_cast<size_t>(nread), buf->len);
  argv[1] = Buffer::Use(env, base, nread);

  Local<Object> pending_obj;
  if (pending == UV_TCP) {
    pending_obj = AcceptHandle<TCPWrap, uv_tcp_t>(env, handle, wrap());
  } else if (pending == UV_NAMED_PIPE) {
    pending_obj = AcceptHandle<PipeWrap, uv_pipe_t>(env, handle, wrap());
  } else if (pending == UV_UDP) {
    pending_obj = AcceptHandle<UDPWrap, uv_udp_t>(env, handle, wrap());
  } else {
    CHECK_EQ(pending, UV_UNKNOWN_HANDLE);
  }

  if (!pending_obj.IsEmpty()) {
    argv[2] = pending_obj;
  }

  wrap()->MakeCallback(env->onread_string(), ARRAY_SIZE(argv), argv);
}


int StreamWrapCallbacks::DoShutdown(ShutdownWrap* req_wrap, uv_shutdown_cb cb) {
  return uv_shutdown(&req_wrap->req_, wrap()->stream(), cb);
}

}  // namespace node

NODE_MODULE_CONTEXT_AWARE_BUILTIN(stream_wrap, node::StreamWrap::Initialize)
