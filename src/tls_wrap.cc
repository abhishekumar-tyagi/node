// Copyright Joyent, Inc. and other Node contributors.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so, subject to the
// following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
// NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
// USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "tls_wrap.h"
#include "async_wrap-inl.h"
#include "node_buffer.h"  // Buffer
#include "node_crypto.h"  // SecureContext
#include "node_crypto_bio.h"  // NodeBIO
// ClientHelloParser
#include "node_crypto_clienthello-inl.h"
#include "node_counters.h"
#include "node_internals.h"
#include "stream_base-inl.h"

namespace node {

using crypto::SecureContext;
using crypto::SSLWrap;
using v8::Context;
using v8::DontDelete;
using v8::EscapableHandleScope;
using v8::Exception;
using v8::Function;
using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::Local;
using v8::Object;
using v8::ReadOnly;
using v8::Signature;
using v8::String;
using v8::Value;

TLSWrap::TLSWrap(Environment* env,
                 Kind kind,
                 StreamBase* stream,
                 SecureContext* sc)
    : AsyncWrap(env,
                env->tls_wrap_constructor_function()
                    ->NewInstance(env->context()).ToLocalChecked(),
                AsyncWrap::PROVIDER_TLSWRAP),
      SSLWrap<TLSWrap>(env, sc, kind),
      StreamBase(env),
      sc_(sc),
      stream_(stream),
      enc_in_(nullptr),
      enc_out_(nullptr),
      write_size_(0),
      started_(false),
      established_(false),
      shutdown_(false),
      cycle_depth_(0),
      eof_(false) {
  node::Wrap(object(), this);
  MakeWeak(this);

  // sc comes from an Unwrap. Make sure it was assigned.
  CHECK_NE(sc, nullptr);

  // We've our own session callbacks
  SSL_CTX_sess_set_get_cb(sc_->ctx_, SSLWrap<TLSWrap>::GetSessionCallback);
  SSL_CTX_sess_set_new_cb(sc_->ctx_, SSLWrap<TLSWrap>::NewSessionCallback);

  stream_->Consume();
  stream_->set_after_write_cb({ OnAfterWriteImpl, this });
  stream_->set_alloc_cb({ OnAllocImpl, this });
  stream_->set_read_cb({ OnReadImpl, this });
  stream_->set_destruct_cb({ OnDestructImpl, this });

  set_alloc_cb({ OnAllocSelf, this });
  set_read_cb({ OnReadSelf, this });

  InitSSL();
}


TLSWrap::~TLSWrap() {
  enc_in_ = nullptr;
  enc_out_ = nullptr;

  sc_ = nullptr;

#ifdef SSL_CTRL_SET_TLSEXT_SERVERNAME_CB
  sni_context_.Reset();
#endif  // SSL_CTRL_SET_TLSEXT_SERVERNAME_CB

  // See test/parallel/test-tls-transport-destroy-after-own-gc.js:
  // If this TLSWrap is garbage collected, we cannot allow callbacks to be
  // called on this stream.

  if (stream_ == nullptr)
    return;
  stream_->set_destruct_cb({ nullptr, nullptr });
  stream_->set_after_write_cb({ nullptr, nullptr });
  stream_->set_alloc_cb({ nullptr, nullptr });
  stream_->set_read_cb({ nullptr, nullptr });
  stream_->set_destruct_cb({ nullptr, nullptr });
  stream_->Unconsume();
}


bool TLSWrap::InvokeQueued(int status, const char* error_str) {
  if (!write_callback_scheduled_)
    return false;

  if (current_write_ != nullptr) {
    WriteWrap* w = current_write_;
    current_write_ = nullptr;
    w->Done(status, error_str);
  }

  return true;
}


void TLSWrap::NewSessionDoneCb() {
  Cycle();
}


void TLSWrap::InitSSL() {
  // Initialize SSL
  enc_in_ = crypto::NodeBIO::New();
  enc_out_ = crypto::NodeBIO::New();
  crypto::NodeBIO::FromBIO(enc_in_)->AssignEnvironment(env());
  crypto::NodeBIO::FromBIO(enc_out_)->AssignEnvironment(env());

  SSL_set_bio(ssl_, enc_in_, enc_out_);

  // NOTE: This could be overridden in SetVerifyMode
  SSL_set_verify(ssl_, SSL_VERIFY_NONE, crypto::VerifyCallback);

#ifdef SSL_MODE_RELEASE_BUFFERS
  long mode = SSL_get_mode(ssl_);  // NOLINT(runtime/int)
  SSL_set_mode(ssl_, mode | SSL_MODE_RELEASE_BUFFERS);
#endif  // SSL_MODE_RELEASE_BUFFERS

  SSL_set_app_data(ssl_, this);
  SSL_set_info_callback(ssl_, SSLInfoCallback);

#ifdef SSL_CTRL_SET_TLSEXT_SERVERNAME_CB
  if (is_server()) {
    SSL_CTX_set_tlsext_servername_callback(sc_->ctx_, SelectSNIContextCallback);
  }
#endif  // SSL_CTRL_SET_TLSEXT_SERVERNAME_CB

  InitNPN(sc_);

  SSL_set_cert_cb(ssl_, SSLWrap<TLSWrap>::SSLCertCallback, this);

  if (is_server()) {
    SSL_set_accept_state(ssl_);
  } else if (is_client()) {
    // Enough space for server response (hello, cert)
    crypto::NodeBIO::FromBIO(enc_in_)->set_initial(kInitialClientBufferLength);
    SSL_set_connect_state(ssl_);
  } else {
    // Unexpected
    ABORT();
  }
}


void TLSWrap::Wrap(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  if (args.Length() < 1 || !args[0]->IsObject()) {
    return env->ThrowTypeError(
        "First argument should be a LibuvStreamWrap instance");
  }
  if (args.Length() < 2 || !args[1]->IsObject()) {
    return env->ThrowTypeError(
        "Second argument should be a SecureContext instance");
  }
  if (args.Length() < 3 || !args[2]->IsBoolean())
    return env->ThrowTypeError("Third argument should be boolean");

  Local<External> stream_obj = args[0].As<External>();
  Local<Object> sc = args[1].As<Object>();
  Kind kind = args[2]->IsTrue() ? SSLWrap<TLSWrap>::kServer :
                                  SSLWrap<TLSWrap>::kClient;

  StreamBase* stream = static_cast<StreamBase*>(stream_obj->Value());
  CHECK_NE(stream, nullptr);

  TLSWrap* res = new TLSWrap(env, kind, stream, Unwrap<SecureContext>(sc));

  args.GetReturnValue().Set(res->object());
}


void TLSWrap::Receive(const FunctionCallbackInfo<Value>& args) {
  TLSWrap* wrap;
  ASSIGN_OR_RETURN_UNWRAP(&wrap, args.Holder());

  CHECK(Buffer::HasInstance(args[0]));
  char* data = Buffer::Data(args[0]);
  size_t len = Buffer::Length(args[0]);

  uv_buf_t buf;

  // Copy given buffer entirely or partiall if handle becomes closed
  while (len > 0 && wrap->IsAlive() && !wrap->IsClosing()) {
    wrap->stream_->EmitAlloc(len, &buf);
    size_t copy = buf.len > len ? len : buf.len;
    memcpy(buf.base, data, copy);
    buf.len = copy;
    wrap->stream_->EmitRead(buf.len, &buf);

    data += copy;
    len -= copy;
  }
}


void TLSWrap::Start(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  TLSWrap* wrap;
  ASSIGN_OR_RETURN_UNWRAP(&wrap, args.Holder());

  if (wrap->started_)
    return env->ThrowError("Already started.");
  wrap->started_ = true;

  // Send ClientHello handshake
  CHECK(wrap->is_client());
  wrap->ClearOut();
  wrap->EncOut();
}


void TLSWrap::SSLInfoCallback(const SSL* ssl_, int where, int ret) {
  if (!(where & (SSL_CB_HANDSHAKE_START | SSL_CB_HANDSHAKE_DONE)))
    return;

  // Be compatible with older versions of OpenSSL. SSL_get_app_data() wants
  // a non-const SSL* in OpenSSL <= 0.9.7e.
  SSL* ssl = const_cast<SSL*>(ssl_);
  TLSWrap* c = static_cast<TLSWrap*>(SSL_get_app_data(ssl));
  Environment* env = c->env();
  Local<Object> object = c->object();

  if (where & SSL_CB_HANDSHAKE_START) {
    Local<Value> callback = object->Get(env->onhandshakestart_string());
    if (callback->IsFunction()) {
      c->MakeCallback(callback.As<Function>(), 0, nullptr);
    }
  }

  if (where & SSL_CB_HANDSHAKE_DONE) {
    c->established_ = true;
    Local<Value> callback = object->Get(env->onhandshakedone_string());
    if (callback->IsFunction()) {
      c->MakeCallback(callback.As<Function>(), 0, nullptr);
    }
  }
}


void TLSWrap::EncOut() {
  // Ignore cycling data if ClientHello wasn't yet parsed
  if (!hello_parser_.IsEnded())
    return;

  // Write in progress
  if (write_size_ != 0)
    return;

  // Wait for `newSession` callback to be invoked
  if (is_waiting_new_session())
    return;

  // Split-off queue
  if (established_ && current_write_ != nullptr)
    write_callback_scheduled_ = true;

  if (ssl_ == nullptr)
    return;

  // No data to write
  if (BIO_pending(enc_out_) == 0) {
    if (pending_cleartext_input_.empty())
      InvokeQueued(0);
    return;
  }

  char* data[kSimultaneousBufferCount];
  size_t size[arraysize(data)];
  size_t count = arraysize(data);
  write_size_ = crypto::NodeBIO::FromBIO(enc_out_)->PeekMultiple(data,
                                                                 size,
                                                                 &count);
  CHECK(write_size_ != 0 && count != 0);

  Local<Object> req_wrap_obj =
      env()->write_wrap_constructor_function()
          ->NewInstance(env()->context()).ToLocalChecked();
  WriteWrap* write_req = WriteWrap::New(env(),
                                        req_wrap_obj,
                                        stream_);

  uv_buf_t buf[arraysize(data)];
  for (size_t i = 0; i < count; i++)
    buf[i] = uv_buf_init(data[i], size[i]);
  int err = stream_->DoWrite(write_req, buf, count, nullptr);

  // Ignore errors, this should be already handled in js
  if (err) {
    write_req->Dispose();
    InvokeQueued(err);
  } else {
    NODE_COUNT_NET_BYTES_SENT(write_size_);
  }
}


void TLSWrap::EncOutAfterWrite(WriteWrap* req_wrap, int status) {
  // We should not be getting here after `DestroySSL`, because all queued writes
  // must be invoked with UV_ECANCELED
  CHECK_NE(ssl_, nullptr);

  // Handle error
  if (status) {
    // Ignore errors after shutdown
    if (shutdown_)
      return;

    // Notify about error
    InvokeQueued(status);
    return;
  }

  // Commit
  crypto::NodeBIO::FromBIO(enc_out_)->Read(nullptr, write_size_);

  // Ensure that the progress will be made and `InvokeQueued` will be called.
  ClearIn();

  // Try writing more data
  write_size_ = 0;
  EncOut();
}


Local<Value> TLSWrap::GetSSLError(int status, int* err, std::string* msg) {
  EscapableHandleScope scope(env()->isolate());

  // ssl_ is already destroyed in reading EOF by close notify alert.
  if (ssl_ == nullptr)
    return Local<Value>();

  *err = SSL_get_error(ssl_, status);
  switch (*err) {
    case SSL_ERROR_NONE:
    case SSL_ERROR_WANT_READ:
    case SSL_ERROR_WANT_WRITE:
    case SSL_ERROR_WANT_X509_LOOKUP:
      break;
    case SSL_ERROR_ZERO_RETURN:
      return scope.Escape(env()->zero_return_string());
      break;
    default:
      {
        CHECK(*err == SSL_ERROR_SSL || *err == SSL_ERROR_SYSCALL);

        BIO* bio = BIO_new(BIO_s_mem());
        ERR_print_errors(bio);

        BUF_MEM* mem;
        BIO_get_mem_ptr(bio, &mem);

        Local<String> message =
            OneByteString(env()->isolate(), mem->data, mem->length);
        Local<Value> exception = Exception::Error(message);

        if (msg != nullptr)
          msg->assign(mem->data, mem->data + mem->length);

        BIO_free_all(bio);

        return scope.Escape(exception);
      }
  }
  return Local<Value>();
}


void TLSWrap::ClearOut() {
  // Ignore cycling data if ClientHello wasn't yet parsed
  if (!hello_parser_.IsEnded())
    return;

  // No reads after EOF
  if (eof_)
    return;

  if (ssl_ == nullptr)
    return;

  crypto::MarkPopErrorOnReturn mark_pop_error_on_return;

  char out[kClearOutChunkSize];
  int read;
  for (;;) {
    read = SSL_read(ssl_, out, sizeof(out));

    if (read <= 0)
      break;

    char* current = out;
    while (read > 0) {
      int avail = read;

      uv_buf_t buf;
      EmitAlloc(avail, &buf);
      if (static_cast<int>(buf.len) < avail)
        avail = buf.len;
      memcpy(buf.base, current, avail);
      EmitRead(avail, &buf);

      // Caveat emptor: OnRead() calls into JS land which can result in
      // the SSL context object being destroyed.  We have to carefully
      // check that ssl_ != nullptr afterwards.
      if (ssl_ == nullptr)
        return;

      read -= avail;
      current += avail;
    }
  }

  int flags = SSL_get_shutdown(ssl_);
  if (!eof_ && flags & SSL_RECEIVED_SHUTDOWN) {
    eof_ = true;
    EmitRead(UV_EOF, nullptr);
  }

  // We need to check whether an error occurred or the connection was
  // shutdown cleanly (SSL_ERROR_ZERO_RETURN) even when read == 0.
  // See node#1642 and SSL_read(3SSL) for details.
  if (read <= 0) {
    int err;
    Local<Value> arg = GetSSLError(read, &err, nullptr);

    // Ignore ZERO_RETURN after EOF, it is basically not a error
    if (err == SSL_ERROR_ZERO_RETURN && eof_)
      return;

    if (!arg.IsEmpty()) {
      // When TLS Alert are stored in wbio,
      // it should be flushed to socket before destroyed.
      if (BIO_pending(enc_out_) != 0)
        EncOut();

      MakeCallback(env()->onerror_string(), 1, &arg);
    }
  }
}


bool TLSWrap::ClearIn() {
  // Ignore cycling data if ClientHello wasn't yet parsed
  if (!hello_parser_.IsEnded())
    return false;

  if (ssl_ == nullptr)
    return false;

  std::vector<uv_buf_t> buffers;
  buffers.swap(pending_cleartext_input_);

  crypto::MarkPopErrorOnReturn mark_pop_error_on_return;

  size_t i;
  int written = 0;
  for (i = 0; i < buffers.size(); ++i) {
    size_t avail = buffers[i].len;
    char* data = buffers[i].base;
    written = SSL_write(ssl_, data, avail);
    CHECK(written == -1 || written == static_cast<int>(avail));
    if (written == -1)
      break;
  }

  // All written
  if (i == buffers.size()) {
    CHECK_GE(written, 0);
    return true;
  }

  // Error or partial write
  int err;
  std::string error_str;
  Local<Value> arg = GetSSLError(written, &err, &error_str);
  if (!arg.IsEmpty()) {
    write_callback_scheduled_ = true;
    InvokeQueued(UV_EPROTO, error_str.c_str());
  } else {
    // Push back the not-yet-written pending buffers into their queue.
    // This can be skipped in the error case because no further writes
    // would succeed anyway.
    pending_cleartext_input_.insert(pending_cleartext_input_.end(),
                                    &buffers[i],
                                    &buffers[buffers.size()]);
  }

  return false;
}


AsyncWrap* TLSWrap::GetAsyncWrap() {
  return static_cast<AsyncWrap*>(this);
}


bool TLSWrap::IsIPCPipe() {
  return stream_->IsIPCPipe();
}


int TLSWrap::GetFD() {
  return stream_->GetFD();
}


bool TLSWrap::IsAlive() {
  return ssl_ != nullptr && stream_ != nullptr && stream_->IsAlive();
}


bool TLSWrap::IsClosing() {
  return stream_->IsClosing();
}



int TLSWrap::ReadStart() {
  if (stream_ != nullptr)
    return stream_->ReadStart();
  return 0;
}


int TLSWrap::ReadStop() {
  if (stream_ != nullptr)
    return stream_->ReadStop();
  return 0;
}


const char* TLSWrap::Error() const {
  return error_.empty() ? nullptr : error_.c_str();
}


void TLSWrap::ClearError() {
  error_.clear();
}


int TLSWrap::DoWrite(WriteWrap* w,
                     uv_buf_t* bufs,
                     size_t count,
                     uv_stream_t* send_handle) {
  CHECK_EQ(send_handle, nullptr);
  CHECK_NE(ssl_, nullptr);

  bool empty = true;

  // Empty writes should not go through encryption process
  size_t i;
  for (i = 0; i < count; i++)
    if (bufs[i].len > 0) {
      empty = false;
      break;
    }
  if (empty) {
    ClearOut();
    // However, if there is any data that should be written to the socket,
    // the callback should not be invoked immediately
    if (BIO_pending(enc_out_) == 0) {
      return stream_->DoWrite(w, bufs, count, send_handle);
    }
  }

  // Store the current write wrap
  CHECK_EQ(current_write_, nullptr);
  current_write_ = w;
  w->Dispatched();

  // Write queued data
  if (empty) {
    EncOut();
    return 0;
  }

  if (ssl_ == nullptr) {
    ClearError();
    error_ = "Write after DestroySSL";
    return UV_EPROTO;
  }

  crypto::MarkPopErrorOnReturn mark_pop_error_on_return;

  int written = 0;
  for (i = 0; i < count; i++) {
    written = SSL_write(ssl_, bufs[i].base, bufs[i].len);
    CHECK(written == -1 || written == static_cast<int>(bufs[i].len));
    if (written == -1)
      break;
  }

  if (i != count) {
    int err;
    Local<Value> arg = GetSSLError(written, &err, &error_);
    if (!arg.IsEmpty())
      return UV_EPROTO;

    pending_cleartext_input_.insert(pending_cleartext_input_.end(),
                                    &bufs[i],
                                    &bufs[count]);
  }

  // Try writing data immediately
  EncOut();

  return 0;
}


void TLSWrap::OnAfterWriteImpl(WriteWrap* w, int status, void* ctx) {
  TLSWrap* wrap = static_cast<TLSWrap*>(ctx);
  wrap->EncOutAfterWrite(w, status);
}


void TLSWrap::OnAllocImpl(size_t suggested_size, uv_buf_t* buf, void* ctx) {
  TLSWrap* wrap = static_cast<TLSWrap*>(ctx);

  if (wrap->ssl_ == nullptr) {
    *buf = uv_buf_init(nullptr, 0);
    return;
  }

  size_t size = 0;
  buf->base = crypto::NodeBIO::FromBIO(wrap->enc_in_)->PeekWritable(&size);
  buf->len = size;
}


void TLSWrap::OnReadImpl(ssize_t nread,
                         const uv_buf_t* buf,
                         uv_handle_type pending,
                         void* ctx) {
  TLSWrap* wrap = static_cast<TLSWrap*>(ctx);
  wrap->DoRead(nread, buf, pending);
}


void TLSWrap::OnDestructImpl(void* ctx) {
  TLSWrap* wrap = static_cast<TLSWrap*>(ctx);
  wrap->clear_stream();
}


void TLSWrap::OnAllocSelf(size_t suggested_size, uv_buf_t* buf, void* ctx) {
  buf->base = node::Malloc(suggested_size);
  buf->len = suggested_size;
}


void TLSWrap::OnReadSelf(ssize_t nread,
                         const uv_buf_t* buf,
                         uv_handle_type pending,
                         void* ctx) {
  TLSWrap* wrap = static_cast<TLSWrap*>(ctx);
  Local<Object> buf_obj;
  if (buf != nullptr)
    buf_obj = Buffer::New(wrap->env(), buf->base, buf->len).ToLocalChecked();
  wrap->EmitData(nread, buf_obj, Local<Object>());
}


void TLSWrap::DoRead(ssize_t nread,
                     const uv_buf_t* buf,
                     uv_handle_type pending) {
  if (nread < 0)  {
    // Error should be emitted only after all data was read
    ClearOut();

    // Ignore EOF if received close_notify
    if (nread == UV_EOF) {
      if (eof_)
        return;
      eof_ = true;
    }

    EmitRead(nread, nullptr);
    return;
  }

  // Only client connections can receive data
  if (ssl_ == nullptr) {
    EmitRead(UV_EPROTO, nullptr);
    return;
  }

  // Commit read data
  crypto::NodeBIO* enc_in = crypto::NodeBIO::FromBIO(enc_in_);
  enc_in->Commit(nread);

  // Parse ClientHello first
  if (!hello_parser_.IsEnded()) {
    size_t avail = 0;
    uint8_t* data = reinterpret_cast<uint8_t*>(enc_in->Peek(&avail));
    CHECK(avail == 0 || data != nullptr);
    return hello_parser_.Parse(data, avail);
  }

  // Cycle OpenSSL's state
  Cycle();
}


int TLSWrap::DoShutdown(ShutdownWrap* req_wrap) {
  crypto::MarkPopErrorOnReturn mark_pop_error_on_return;

  if (ssl_ != nullptr && SSL_shutdown(ssl_) == 0)
    SSL_shutdown(ssl_);

  shutdown_ = true;
  EncOut();
  return stream_->DoShutdown(req_wrap);
}


void TLSWrap::SetVerifyMode(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  TLSWrap* wrap;
  ASSIGN_OR_RETURN_UNWRAP(&wrap, args.Holder());

  if (args.Length() < 2 || !args[0]->IsBoolean() || !args[1]->IsBoolean())
    return env->ThrowTypeError("Bad arguments, expected two booleans");

  if (wrap->ssl_ == nullptr)
    return env->ThrowTypeError("SetVerifyMode after destroySSL");

  int verify_mode;
  if (wrap->is_server()) {
    bool request_cert = args[0]->IsTrue();
    if (!request_cert) {
      // Note reject_unauthorized ignored.
      verify_mode = SSL_VERIFY_NONE;
    } else {
      bool reject_unauthorized = args[1]->IsTrue();
      verify_mode = SSL_VERIFY_PEER;
      if (reject_unauthorized)
        verify_mode |= SSL_VERIFY_FAIL_IF_NO_PEER_CERT;
    }
  } else {
    // Note request_cert and reject_unauthorized are ignored for clients.
    verify_mode = SSL_VERIFY_NONE;
  }

  // Always allow a connection. We'll reject in javascript.
  SSL_set_verify(wrap->ssl_, verify_mode, crypto::VerifyCallback);
}


void TLSWrap::EnableSessionCallbacks(
    const FunctionCallbackInfo<Value>& args) {
  TLSWrap* wrap;
  ASSIGN_OR_RETURN_UNWRAP(&wrap, args.Holder());
  if (wrap->ssl_ == nullptr) {
    return wrap->env()->ThrowTypeError(
        "EnableSessionCallbacks after destroySSL");
  }
  wrap->enable_session_callbacks();
  crypto::NodeBIO::FromBIO(wrap->enc_in_)->set_initial(kMaxHelloLength);
  wrap->hello_parser_.Start(SSLWrap<TLSWrap>::OnClientHello,
                            OnClientHelloParseEnd,
                            wrap);
}


void TLSWrap::DestroySSL(const FunctionCallbackInfo<Value>& args) {
  TLSWrap* wrap;
  ASSIGN_OR_RETURN_UNWRAP(&wrap, args.Holder());

  // If there is a write happening, mark it as finished.
  wrap->write_callback_scheduled_ = true;

  // And destroy
  wrap->InvokeQueued(UV_ECANCELED, "Canceled because of SSL destruction");

  // Destroy the SSL structure and friends
  wrap->SSLWrap<TLSWrap>::DestroySSL();
}


void TLSWrap::EnableCertCb(const FunctionCallbackInfo<Value>& args) {
  TLSWrap* wrap;
  ASSIGN_OR_RETURN_UNWRAP(&wrap, args.Holder());
  wrap->WaitForCertCb(OnClientHelloParseEnd, wrap);
}


void TLSWrap::OnClientHelloParseEnd(void* arg) {
  TLSWrap* c = static_cast<TLSWrap*>(arg);
  c->Cycle();
}


#ifdef SSL_CTRL_SET_TLSEXT_SERVERNAME_CB
void TLSWrap::GetServername(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  TLSWrap* wrap;
  ASSIGN_OR_RETURN_UNWRAP(&wrap, args.Holder());

  CHECK_NE(wrap->ssl_, nullptr);

  const char* servername = SSL_get_servername(wrap->ssl_,
                                              TLSEXT_NAMETYPE_host_name);
  if (servername != nullptr) {
    args.GetReturnValue().Set(OneByteString(env->isolate(), servername));
  } else {
    args.GetReturnValue().Set(false);
  }
}


void TLSWrap::SetServername(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  TLSWrap* wrap;
  ASSIGN_OR_RETURN_UNWRAP(&wrap, args.Holder());

  if (args.Length() < 1 || !args[0]->IsString())
    return env->ThrowTypeError("First argument should be a string");

  if (wrap->started_)
    return env->ThrowError("Already started.");

  if (!wrap->is_client())
    return;

  CHECK_NE(wrap->ssl_, nullptr);

#ifdef SSL_CTRL_SET_TLSEXT_SERVERNAME_CB
  node::Utf8Value servername(env->isolate(), args[0].As<String>());
  SSL_set_tlsext_host_name(wrap->ssl_, *servername);
#endif  // SSL_CTRL_SET_TLSEXT_SERVERNAME_CB
}


int TLSWrap::SelectSNIContextCallback(SSL* s, int* ad, void* arg) {
  TLSWrap* p = static_cast<TLSWrap*>(SSL_get_app_data(s));
  Environment* env = p->env();

  const char* servername = SSL_get_servername(s, TLSEXT_NAMETYPE_host_name);

  if (servername == nullptr)
    return SSL_TLSEXT_ERR_OK;

  // Call the SNI callback and use its return value as context
  Local<Object> object = p->object();
  Local<Value> ctx = object->Get(env->sni_context_string());

  // Not an object, probably undefined or null
  if (!ctx->IsObject())
    return SSL_TLSEXT_ERR_NOACK;

  Local<FunctionTemplate> cons = env->secure_context_constructor_template();
  if (!cons->HasInstance(ctx)) {
    // Failure: incorrect SNI context object
    Local<Value> err = Exception::TypeError(env->sni_context_err_string());
    p->MakeCallback(env->onerror_string(), 1, &err);
    return SSL_TLSEXT_ERR_NOACK;
  }

  p->sni_context_.Reset();
  p->sni_context_.Reset(env->isolate(), ctx);

  SecureContext* sc = Unwrap<SecureContext>(ctx.As<Object>());
  CHECK_NE(sc, nullptr);
  p->SetSNIContext(sc);
  return SSL_TLSEXT_ERR_OK;
}
#endif  // SSL_CTRL_SET_TLSEXT_SERVERNAME_CB


void TLSWrap::GetWriteQueueSize(const FunctionCallbackInfo<Value>& info) {
  TLSWrap* wrap;
  ASSIGN_OR_RETURN_UNWRAP(&wrap, info.This());

  if (wrap->ssl_ == nullptr) {
    info.GetReturnValue().Set(0);
    return;
  }

  uint32_t write_queue_size = BIO_pending(wrap->enc_out_);
  info.GetReturnValue().Set(write_queue_size);
}


void TLSWrap::Initialize(Local<Object> target,
                         Local<Value> unused,
                         Local<Context> context) {
  Environment* env = Environment::GetCurrent(context);

  env->SetMethod(target, "wrap", TLSWrap::Wrap);

  auto constructor = [](const FunctionCallbackInfo<Value>& args) {
    CHECK(args.IsConstructCall());
    args.This()->SetAlignedPointerInInternalField(0, nullptr);
  };

  Local<String> tlsWrapString =
      FIXED_ONE_BYTE_STRING(env->isolate(), "TLSWrap");

  auto t = env->NewFunctionTemplate(constructor);
  t->InstanceTemplate()->SetInternalFieldCount(1);
  t->SetClassName(tlsWrapString);

  Local<FunctionTemplate> get_write_queue_size =
      FunctionTemplate::New(env->isolate(),
                            GetWriteQueueSize,
                            env->as_external(),
                            Signature::New(env->isolate(), t));
  t->PrototypeTemplate()->SetAccessorProperty(
      env->write_queue_size_string(),
      get_write_queue_size,
      Local<FunctionTemplate>(),
      static_cast<PropertyAttribute>(ReadOnly | DontDelete));

  AsyncWrap::AddWrapMethods(env, t, AsyncWrap::kFlagHasReset);
  env->SetProtoMethod(t, "receive", Receive);
  env->SetProtoMethod(t, "start", Start);
  env->SetProtoMethod(t, "setVerifyMode", SetVerifyMode);
  env->SetProtoMethod(t, "enableSessionCallbacks", EnableSessionCallbacks);
  env->SetProtoMethod(t, "destroySSL", DestroySSL);
  env->SetProtoMethod(t, "enableCertCb", EnableCertCb);

  StreamBase::AddMethods<TLSWrap>(env, t, StreamBase::kFlagHasWritev);
  SSLWrap<TLSWrap>::AddMethods(env, t);

#ifdef SSL_CTRL_SET_TLSEXT_SERVERNAME_CB
  env->SetProtoMethod(t, "getServername", GetServername);
  env->SetProtoMethod(t, "setServername", SetServername);
#endif  // SSL_CRT_SET_TLSEXT_SERVERNAME_CB

  env->set_tls_wrap_constructor_function(t->GetFunction());

  target->Set(tlsWrapString, t->GetFunction());
}

}  // namespace node

NODE_BUILTIN_MODULE_CONTEXT_AWARE(tls_wrap, node::TLSWrap::Initialize)
