#include "stream_base.h"

#include "node.h"
#include "node_buffer.h"
#include "env.h"
#include "env-inl.h"
#include "stream_wrap.h"
#include "string_bytes.h"
#include "util.h"
#include "util-inl.h"
#include "v8.h"

#include <limits.h>  // INT_MAX

namespace node {

using v8::Array;
using v8::Context;
using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::Handle;
using v8::HandleScope;
using v8::Integer;
using v8::Local;
using v8::Number;
using v8::Object;
using v8::PropertyAttribute;
using v8::PropertyCallbackInfo;
using v8::String;
using v8::Value;

template void StreamBase::AddMethods<StreamWrap>(Environment* env,
                                                 Handle<FunctionTemplate> t);


StreamBase::StreamBase() : consumed_by_(nullptr) {
}


template <class Base>
void StreamBase::AddMethods(Environment* env, Handle<FunctionTemplate> t) {
  HandleScope scope(env->isolate());

  enum PropertyAttribute attributes =
      static_cast<PropertyAttribute>(v8::ReadOnly | v8::DontDelete);
  t->InstanceTemplate()->SetAccessor(env->fd_string(),
                                     GetFD<Base>,
                                     nullptr,
                                     Handle<Value>(),
                                     v8::DEFAULT,
                                     attributes);

  env->SetProtoMethod(t, "readStart", JSMethod<Base, &StreamBase::ReadStart>);
  env->SetProtoMethod(t, "readStop", JSMethod<Base, &StreamBase::ReadStop>);
  env->SetProtoMethod(t, "shutdown", JSMethod<Base, &StreamBase::Shutdown>);
  env->SetProtoMethod(t, "writev", JSMethod<Base, &StreamBase::Writev>);
  env->SetProtoMethod(t,
                      "writeBuffer",
                      JSMethod<Base, &StreamBase::WriteBuffer>);
  env->SetProtoMethod(t,
                      "writeAsciiString",
                      JSMethod<Base, &StreamBase::WriteString<ASCII> >);
  env->SetProtoMethod(t,
                      "writeUtf8String",
                      JSMethod<Base, &StreamBase::WriteString<UTF8> >);
  env->SetProtoMethod(t,
                      "writeUcs2String",
                      JSMethod<Base, &StreamBase::WriteString<UCS2> >);
  env->SetProtoMethod(t,
                      "writeBinaryString",
                      JSMethod<Base, &StreamBase::WriteString<BINARY> >);
  env->SetProtoMethod(t,
                      "setBlocking",
                      JSMethod<Base, &StreamBase::SetBlocking>);
}


template <class Base>
void StreamBase::GetFD(Local<String> key,
                       const PropertyCallbackInfo<Value>& args) {
  HandleScope scope(args.GetIsolate());
  StreamBase* wrap = Unwrap<Base>(args.Holder());

  while (wrap->ConsumedBy() != nullptr)
    wrap = wrap->ConsumedBy();

  if (!wrap->IsAlive())
    return args.GetReturnValue().Set(UV_EINVAL);

  args.GetReturnValue().Set(wrap->GetFD());
}


template <class Base,
          int (StreamBase::*Method)(const FunctionCallbackInfo<Value>& args)>
void StreamBase::JSMethod(const FunctionCallbackInfo<Value>& args) {
  HandleScope scope(args.GetIsolate());
  StreamBase* wrap = Unwrap<Base>(args.Holder());

  while (wrap->ConsumedBy() != nullptr)
    wrap = wrap->ConsumedBy();

  if (!wrap->IsAlive())
    return args.GetReturnValue().Set(UV_EINVAL);

  args.GetReturnValue().Set((wrap->*Method)(args));
}


int StreamBase::ReadStart(const v8::FunctionCallbackInfo<v8::Value>& args) {
  return ReadStart();
}


int StreamBase::ReadStop(const v8::FunctionCallbackInfo<v8::Value>& args) {
  return ReadStop();
}


int StreamBase::Shutdown(const v8::FunctionCallbackInfo<v8::Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  CHECK(args[0]->IsObject());
  Local<Object> req_wrap_obj = args[0].As<Object>();

  ShutdownWrap* req_wrap = new ShutdownWrap(env, req_wrap_obj, this);
  int err = DoShutdown(req_wrap, AfterShutdown);
  req_wrap->Dispatched();
  if (err)
    delete req_wrap;
  return err;
}


void StreamBase::AfterShutdown(uv_shutdown_t* req, int status) {
  ShutdownWrap* req_wrap = static_cast<ShutdownWrap*>(req->data);
  StreamBase* wrap = req_wrap->wrap();
  Environment* env = req_wrap->env();

  // The wrap and request objects should still be there.
  CHECK_EQ(req_wrap->persistent().IsEmpty(), false);
  CHECK_EQ(wrap->GetObject().IsEmpty(), false);

  HandleScope handle_scope(env->isolate());
  Context::Scope context_scope(env->context());

  Local<Object> req_wrap_obj = req_wrap->object();
  Local<Value> argv[3] = {
    Integer::New(env->isolate(), status),
    wrap->GetObject(),
    req_wrap_obj
  };

  req_wrap->MakeCallback(env->oncomplete_string(), ARRAY_SIZE(argv), argv);

  delete req_wrap;
}


int StreamBase::Writev(const v8::FunctionCallbackInfo<v8::Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  CHECK(args[0]->IsObject());
  CHECK(args[1]->IsArray());

  Local<Object> req_wrap_obj = args[0].As<Object>();
  Local<Array> chunks = args[1].As<Array>();

  size_t count = chunks->Length() >> 1;

  uv_buf_t bufs_[16];
  uv_buf_t* bufs = bufs_;

  // Determine storage size first
  size_t storage_size = 0;
  for (size_t i = 0; i < count; i++) {
    Handle<Value> chunk = chunks->Get(i * 2);

    if (Buffer::HasInstance(chunk))
      continue;
      // Buffer chunk, no additional storage required

    // String chunk
    Handle<String> string = chunk->ToString(env->isolate());
    enum encoding encoding = ParseEncoding(env->isolate(),
                                           chunks->Get(i * 2 + 1));
    size_t chunk_size;
    if (encoding == UTF8 && string->Length() > 65535)
      chunk_size = StringBytes::Size(env->isolate(), string, encoding);
    else
      chunk_size = StringBytes::StorageSize(env->isolate(), string, encoding);

    storage_size += chunk_size + 15;
  }

  if (storage_size > INT_MAX)
    return UV_ENOBUFS;

  if (ARRAY_SIZE(bufs_) < count)
    bufs = new uv_buf_t[count];

  storage_size += sizeof(WriteWrap);
  char* storage = new char[storage_size];
  WriteWrap* req_wrap =
      new(storage) WriteWrap(env, req_wrap_obj, this);

  uint32_t bytes = 0;
  size_t offset = sizeof(WriteWrap);
  for (size_t i = 0; i < count; i++) {
    Handle<Value> chunk = chunks->Get(i * 2);

    // Write buffer
    if (Buffer::HasInstance(chunk)) {
      bufs[i].base = Buffer::Data(chunk);
      bufs[i].len = Buffer::Length(chunk);
      bytes += bufs[i].len;
      continue;
    }

    // Write string
    offset = ROUND_UP(offset, 16);
    CHECK_LT(offset, storage_size);
    char* str_storage = storage + offset;
    size_t str_size = storage_size - offset;

    Handle<String> string = chunk->ToString(env->isolate());
    enum encoding encoding = ParseEncoding(env->isolate(),
                                           chunks->Get(i * 2 + 1));
    str_size = StringBytes::Write(env->isolate(),
                                  str_storage,
                                  str_size,
                                  string,
                                  encoding);
    bufs[i].base = str_storage;
    bufs[i].len = str_size;
    offset += str_size;
    bytes += str_size;
  }

  int err = DoWrite(req_wrap,
                    bufs,
                    count,
                    nullptr,
                    StreamBase::AfterWrite);

  // Deallocate space
  if (bufs != bufs_)
    delete[] bufs;

  req_wrap->Dispatched();
  req_wrap->object()->Set(env->async(), True(env->isolate()));
  req_wrap->object()->Set(env->bytes_string(),
                          Number::New(env->isolate(), bytes));
  const char* msg = Error();
  if (msg != nullptr) {
    req_wrap_obj->Set(env->error_string(), OneByteString(env->isolate(), msg));
    ClearError();
  }

  if (err) {
    req_wrap->~WriteWrap();
    delete[] storage;
  }

  return err;
}




int StreamBase::WriteBuffer(const v8::FunctionCallbackInfo<v8::Value>& args) {
  CHECK(args[0]->IsObject());
  CHECK(Buffer::HasInstance(args[1]));
  Environment* env = Environment::GetCurrent(args);

  Local<Object> req_wrap_obj = args[0].As<Object>();
  const char* data = Buffer::Data(args[1]);
  size_t length = Buffer::Length(args[1]);

  char* storage;
  WriteWrap* req_wrap;
  uv_buf_t buf;
  buf.base = const_cast<char*>(data);
  buf.len = length;

  // Try writing immediately without allocation
  uv_buf_t* bufs = &buf;
  size_t count = 1;
  int err = DoTryWrite(&bufs, &count);
  if (err != 0)
    goto done;
  if (count == 0)
    goto done;
  CHECK_EQ(count, 1);

  // Allocate, or write rest
  storage = new char[sizeof(WriteWrap)];
  req_wrap = new(storage) WriteWrap(env, req_wrap_obj, this);

  err = DoWrite(req_wrap,
                bufs,
                count,
                nullptr,
                StreamBase::AfterWrite);
  req_wrap->Dispatched();
  req_wrap_obj->Set(env->async(), True(env->isolate()));

  if (err) {
    req_wrap->~WriteWrap();
    delete[] storage;
  }

 done:
  const char* msg = Error();
  if (msg != nullptr) {
    req_wrap_obj->Set(env->error_string(), OneByteString(env->isolate(), msg));
    ClearError();
  }
  req_wrap_obj->Set(env->bytes_string(),
                    Integer::NewFromUnsigned(env->isolate(), length));
  return err;
}


template <enum encoding enc>
int StreamBase::WriteString(const v8::FunctionCallbackInfo<v8::Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  CHECK(args[0]->IsObject());
  CHECK(args[1]->IsString());

  Local<Object> req_wrap_obj = args[0].As<Object>();
  Local<String> string = args[1].As<String>();
  Local<Object> send_handle_obj;
  if (args[2]->IsObject())
    send_handle_obj = args[2].As<Object>();

  int err;

  // Compute the size of the storage that the string will be flattened into.
  // For UTF8 strings that are very long, go ahead and take the hit for
  // computing their actual size, rather than tripling the storage.
  size_t storage_size;
  if (enc == UTF8 && string->Length() > 65535)
    storage_size = StringBytes::Size(env->isolate(), string, enc);
  else
    storage_size = StringBytes::StorageSize(env->isolate(), string, enc);

  if (storage_size > INT_MAX)
    return UV_ENOBUFS;

  // Try writing immediately if write size isn't too big
  char* storage;
  WriteWrap* req_wrap;
  char* data;
  char stack_storage[16384];  // 16kb
  size_t data_size;
  uv_buf_t buf;

  bool try_write = storage_size + 15 <= sizeof(stack_storage) &&
                   (!IsIPCPipe() || send_handle_obj.IsEmpty());
  if (try_write) {
    data_size = StringBytes::Write(env->isolate(),
                                   stack_storage,
                                   storage_size,
                                   string,
                                   enc);
    buf = uv_buf_init(stack_storage, data_size);

    uv_buf_t* bufs = &buf;
    size_t count = 1;
    err = DoTryWrite(&bufs, &count);

    // Failure
    if (err != 0)
      goto done;

    // Success
    if (count == 0)
      goto done;

    // Partial write
    CHECK_EQ(count, 1);
  }

  storage = new char[sizeof(WriteWrap) + storage_size + 15];
  req_wrap = new(storage) WriteWrap(env, req_wrap_obj, this);

  data = reinterpret_cast<char*>(ROUND_UP(
      reinterpret_cast<uintptr_t>(storage) + sizeof(WriteWrap), 16));

  if (try_write) {
    // Copy partial data
    memcpy(data, buf.base, buf.len);
    data_size = buf.len;
  } else {
    // Write it
    data_size = StringBytes::Write(env->isolate(),
                                   data,
                                   storage_size,
                                   string,
                                   enc);
  }

  CHECK_LE(data_size, storage_size);

  buf = uv_buf_init(data, data_size);

  if (!IsIPCPipe()) {
    err = DoWrite(req_wrap,
                  &buf,
                  1,
                  nullptr,
                  StreamBase::AfterWrite);
  } else {
    uv_handle_t* send_handle = nullptr;

    if (!send_handle_obj.IsEmpty()) {
      HandleWrap* wrap = Unwrap<HandleWrap>(send_handle_obj);
      send_handle = wrap->GetHandle();
      // Reference StreamWrap instance to prevent it from being garbage
      // collected before `AfterWrite` is called.
      CHECK_EQ(false, req_wrap->persistent().IsEmpty());
      req_wrap->object()->Set(env->handle_string(), send_handle_obj);
    }

    err = DoWrite(
        req_wrap,
        &buf,
        1,
        reinterpret_cast<uv_stream_t*>(send_handle),
        StreamBase::AfterWrite);
  }

  req_wrap->Dispatched();
  req_wrap->object()->Set(env->async(), True(env->isolate()));

  if (err) {
    req_wrap->~WriteWrap();
    delete[] storage;
  }

 done:
  const char* msg = Error();
  if (msg != nullptr) {
    req_wrap_obj->Set(env->error_string(), OneByteString(env->isolate(), msg));
    ClearError();
  }
  req_wrap_obj->Set(env->bytes_string(),
                    Integer::NewFromUnsigned(env->isolate(), data_size));
  return err;
}


void StreamBase::AfterWrite(uv_write_t* req, int status) {
  WriteWrap* req_wrap = ContainerOf(&WriteWrap::req_, req);
  StreamBase* wrap = req_wrap->wrap();
  Environment* env = req_wrap->env();

  HandleScope handle_scope(env->isolate());
  Context::Scope context_scope(env->context());

  // The wrap and request objects should still be there.
  CHECK_EQ(req_wrap->persistent().IsEmpty(), false);
  CHECK_EQ(wrap->GetObject().IsEmpty(), false);

  // Unref handle property
  Local<Object> req_wrap_obj = req_wrap->object();
  req_wrap_obj->Delete(env->handle_string());
  wrap->OnAfterWrite(req_wrap);

  Local<Value> argv[] = {
    Integer::New(env->isolate(), status),
    wrap->GetObject(),
    req_wrap_obj,
    Undefined(env->isolate())
  };

  const char* msg = wrap->Error();
  if (msg != nullptr) {
    argv[3] = OneByteString(env->isolate(), msg);
    wrap->ClearError();
  }

  req_wrap->MakeCallback(env->oncomplete_string(), ARRAY_SIZE(argv), argv);

  req_wrap->~WriteWrap();
  delete[] reinterpret_cast<char*>(req_wrap);
}


int StreamBase::SetBlocking(const v8::FunctionCallbackInfo<v8::Value>& args) {
  CHECK_GT(args.Length(), 0);
  return SetBlocking(args[0]->IsTrue());
}

}  // namespace node
