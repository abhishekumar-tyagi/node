#include "node.h"
#include "node_buffer.h"

#include "async-wrap.h"
#include "async-wrap-inl.h"
#include "env.h"
#include "env-inl.h"
#include "util.h"
#include "util-inl.h"

#include "v8.h"
#include "zlib.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

namespace node {

using v8::Array;
using v8::Boolean;
using v8::Context;
using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::HandleScope;
using v8::Int32;
using v8::Integer;
using v8::Local;
using v8::MaybeLocal;
using v8::Number;
using v8::Null;
using v8::Object;
using v8::String;
using v8::Value;

enum node_zlib_mode {
  NONE,
  DEFLATE,
  INFLATE,
  GZIP,
  GUNZIP,
  DEFLATERAW,
  INFLATERAW,
  UNZIP
};

#define GZIP_HEADER_ID1 0x1f
#define GZIP_HEADER_ID2 0x8b

void InitZlib(v8::Local<v8::Object> target);

class GZipHeader {
  public:
    explicit GZipHeader(Environment* env) {
      buf_.done = 0;
      buf_.extra_len = buf_.extra_max = 0x10000;
      buf_.name_max = 1024;  // This is what gunzip(1) does.
      buf_.comm_max = 0x10000;

      buf_.extra = extra_ptr_ = static_cast<Bytef*>(malloc(buf_.extra_max));
      buf_.name = name_ptr_ = static_cast<Bytef*>(malloc(buf_.name_max));
      buf_.comment = comment_ptr = static_cast<Bytef*>(malloc(buf_.comm_max));

      env->isolate()->AdjustAmountOfExternalAllocatedMemory(memoryUsage());
    }

    GZipHeader(Environment* env, Local<Object> info) {
      buf_.done = 1;
      buf_.text = info->Get(env->text_string())->Int32Value();
      buf_.time = info->Get(env->time_string())->IntegerValue();
      buf_.os = info->Get(env->os_string())->Int32Value();
      buf_.hcrc = info->Get(env->hcrc_string())->Int32Value();

      readMaybeBuffer(env, info, env->extra_string(),
                      buf_.extra, buf_.extra_len);
      readMaybeBuffer(env, info, env->name_string(),
                      buf_.name, buf_.name_max);
      readMaybeBuffer(env, info, env->comment_string(),
                      buf_.comment, buf_.comm_max);

      extra_ptr_ = buf_.extra;
      name_ptr_ = buf_.name;
      comment_ptr = buf_.comment;

      env->isolate()->AdjustAmountOfExternalAllocatedMemory(memoryUsage());
    }

    bool hasBeenRead() const {
      return buf_.done == 1;
    }

    void resetHasBeenRead() {
      buf_.done = 0;
    }

    Local<Object> toObject(Environment* env) {
      Local<Object> ret = Object::New(env->isolate());

      ret->Set(env->text_string(), Boolean::New(env->isolate(), buf_.text));
      ret->Set(env->time_string(), Int32::New(env->isolate(), buf_.time));
      ret->Set(env->os_string(), Int32::New(env->isolate(), buf_.os));
      ret->Set(env->hcrc_string(), Boolean::New(env->isolate(), buf_.hcrc));

      SetFromBufferOrNull<false>(env, ret, env->extra_string(),
                                 buf_.extra, buf_.extra_len);
      SetFromBufferOrNull<true>(env, ret, env->name_string(),
                                buf_.name, buf_.name_max);
      SetFromBufferOrNull<true>(env, ret, env->comment_string(),
                                buf_.comment, buf_.comm_max);

      return ret;
    }

    void cleanup(Environment* env) {
      int64_t freedMemory = memoryUsage();

      free(extra_ptr_);
      buf_.extra = extra_ptr_ = nullptr;
      buf_.extra_len = buf_.extra_max = 0;

      free(name_ptr_);
      buf_.name = name_ptr_ = nullptr;
      buf_.name_max = 0;

      free(comment_ptr);
      buf_.comment = comment_ptr = nullptr;
      buf_.comm_max = 0;

      if (env) {
        env->isolate()->AdjustAmountOfExternalAllocatedMemory(-freedMemory);
      }
    }

    ~GZipHeader() {
      // This should be a no-op.
      cleanup(nullptr);
    }

    gz_header* underlying() {
      return &buf_;
    }
  private:
    template<bool ZeroTerminated>
    static void SetFromBufferOrNull(Environment* env,
                                    Local<Object> target,
                                    Local<String> name,
                                    Bytef* data_,
                                    uInt length) {
      const char* data = reinterpret_cast<const char*>(data_);
      if (!data) {
        target->Set(name, Null(env->isolate()));
        return;
      }

      if (ZeroTerminated) {
        length = strnlen(data, length);
      }

      MaybeLocal<Object> buf = Buffer::Copy(env, data, length);
      if (!buf.IsEmpty()) {
        target->Set(name, buf.ToLocalChecked());
      }
    }

    void readMaybeBuffer(Environment* env,
                         Local<Object> info,
                         Local<String> name,
                         Bytef*& target,
                         uInt& len_target) {
      Local<Value> source = info->Get(name);

      target = nullptr;
      len_target = 0;

      if (Buffer::HasInstance(source)) {
        len_target = Buffer::Length(source);
        target = static_cast<Bytef*>(malloc(len_target));

        if (target) {
          memcpy(target, Buffer::Data(source), len_target);
        } else {
          len_target = 0;
        }
      }
    }

    int64_t memoryUsage() {
      return buf_.extra_len + buf_.name_max + buf_.comm_max;
    }

    gz_header buf_;
    Bytef* extra_ptr_;
    Bytef* name_ptr_;
    Bytef* comment_ptr;
};

/**
 * Deflate/Inflate
 */
class ZCtx : public AsyncWrap {
 public:

  ZCtx(Environment* env, Local<Object> wrap, node_zlib_mode mode)
      : AsyncWrap(env, wrap, AsyncWrap::PROVIDER_ZLIB),
        chunk_size_(0),
        dictionary_(nullptr),
        dictionary_len_(0),
        err_(0),
        flush_(0),
        init_done_(false),
        level_(0),
        memLevel_(0),
        mode_(mode),
        strategy_(0),
        windowBits_(0),
        gzip_header_(nullptr),
        write_in_progress_(false),
        pending_close_(false),
        refs_(0),
        gzip_id_bytes_read_(0) {
    MakeWeak<ZCtx>(this);
  }


  ~ZCtx() override {
    CHECK_EQ(false, write_in_progress_ && "write in progress");
    Close();
  }

  void Close() {
    if (write_in_progress_) {
      pending_close_ = true;
      return;
    }

    pending_close_ = false;
    CHECK(init_done_ && "close before init");
    CHECK_LE(mode_, UNZIP);

    if (mode_ == DEFLATE || mode_ == GZIP || mode_ == DEFLATERAW) {
      (void)deflateEnd(&strm_);
      int64_t change_in_bytes = -static_cast<int64_t>(kDeflateContextSize);
      env()->isolate()->AdjustAmountOfExternalAllocatedMemory(change_in_bytes);
    } else if (mode_ == INFLATE || mode_ == GUNZIP || mode_ == INFLATERAW ||
               mode_ == UNZIP) {
      (void)inflateEnd(&strm_);
      int64_t change_in_bytes = -static_cast<int64_t>(kInflateContextSize);
      env()->isolate()->AdjustAmountOfExternalAllocatedMemory(change_in_bytes);
    }
    mode_ = NONE;

    if (dictionary_ != nullptr) {
      delete[] dictionary_;
      dictionary_ = nullptr;
    }

    delete gzip_header_;
    gzip_header_ = nullptr;
  }


  static void Close(const FunctionCallbackInfo<Value>& args) {
    ZCtx* ctx = Unwrap<ZCtx>(args.Holder());
    ctx->Close();
  }


  // write(flush, in, in_off, in_len, out, out_off, out_len)
  template <bool async>
  static void Write(const FunctionCallbackInfo<Value>& args) {
    CHECK_EQ(args.Length(), 7);

    ZCtx* ctx = Unwrap<ZCtx>(args.Holder());
    CHECK(ctx->init_done_ && "write before init");
    CHECK(ctx->mode_ != NONE && "already finalized");

    CHECK_EQ(false, ctx->write_in_progress_ && "write already in progress");
    CHECK_EQ(false, ctx->pending_close_ && "close is pending");
    ctx->write_in_progress_ = true;
    ctx->Ref();

    CHECK_EQ(false, args[0]->IsUndefined() && "must provide flush value");

    unsigned int flush = args[0]->Uint32Value();

    if (flush != Z_NO_FLUSH &&
        flush != Z_PARTIAL_FLUSH &&
        flush != Z_SYNC_FLUSH &&
        flush != Z_FULL_FLUSH &&
        flush != Z_FINISH &&
        flush != Z_BLOCK) {
      CHECK(0 && "Invalid flush value");
    }

    Bytef *in;
    Bytef *out;
    size_t in_off, in_len, out_off, out_len;
    Environment* env = ctx->env();

    if (args[1]->IsNull()) {
      // just a flush
      Bytef nada[1] = { 0 };
      in = nada;
      in_len = 0;
      in_off = 0;
    } else {
      CHECK(Buffer::HasInstance(args[1]));
      Local<Object> in_buf;
      in_buf = args[1]->ToObject(env->isolate());
      in_off = args[2]->Uint32Value();
      in_len = args[3]->Uint32Value();

      CHECK(Buffer::IsWithinBounds(in_off, in_len, Buffer::Length(in_buf)));
      in = reinterpret_cast<Bytef *>(Buffer::Data(in_buf) + in_off);
    }

    CHECK(Buffer::HasInstance(args[4]));
    Local<Object> out_buf = args[4]->ToObject(env->isolate());
    out_off = args[5]->Uint32Value();
    out_len = args[6]->Uint32Value();
    CHECK(Buffer::IsWithinBounds(out_off, out_len, Buffer::Length(out_buf)));
    out = reinterpret_cast<Bytef *>(Buffer::Data(out_buf) + out_off);

    // build up the work request
    uv_work_t* work_req = &(ctx->work_req_);

    ctx->strm_.avail_in = in_len;
    ctx->strm_.next_in = in;
    ctx->strm_.avail_out = out_len;
    ctx->strm_.next_out = out;
    ctx->flush_ = flush;

    // set this so that later on, I can easily tell how much was written.
    ctx->chunk_size_ = out_len;

    if (!async) {
      // sync version
      ctx->env()->PrintSyncTrace();
      Process(work_req);
      if (CheckError(ctx))
        AfterSync(ctx, args);
      return;
    }

    // async version
    uv_queue_work(ctx->env()->event_loop(),
                  work_req,
                  ZCtx::Process,
                  ZCtx::After);

    args.GetReturnValue().Set(ctx->object());
  }


  static void AfterSync(ZCtx* ctx, const FunctionCallbackInfo<Value>& args) {
    Environment* env = ctx->env();
    Local<Integer> avail_out = Integer::New(env->isolate(),
                                            ctx->strm_.avail_out);
    Local<Integer> avail_in = Integer::New(env->isolate(),
                                           ctx->strm_.avail_in);

    ctx->write_in_progress_ = false;

    Local<Array> result = Array::New(env->isolate(), 3);
    result->Set(0, avail_in);
    result->Set(1, avail_out);
    if (ctx->gzip_header_ && ctx->gzip_header_->hasBeenRead()) {
      result->Set(2, ctx->gzip_header_->toObject(env));
      ctx->gzip_header_->resetHasBeenRead();
    } else {
      result->Set(2, Null(env->isolate()));
    }

    args.GetReturnValue().Set(result);

    ctx->Unref();
  }


  // thread pool!
  // This function may be called multiple times on the uv_work pool
  // for a single write() call, until all of the input bytes have
  // been consumed.
  static void Process(uv_work_t* work_req) {
    ZCtx *ctx = ContainerOf(&ZCtx::work_req_, work_req);

    const Bytef* next_expected_header_byte = nullptr;

    // If the avail_out is left at 0, then it means that it ran out
    // of room.  If there was avail_out left over, then it means
    // that all of the input was consumed.
    switch (ctx->mode_) {
      case DEFLATE:
      case GZIP:
      case DEFLATERAW:
        ctx->err_ = deflate(&ctx->strm_, ctx->flush_);
        break;
      case UNZIP:
        if (ctx->strm_.avail_in > 0) {
          next_expected_header_byte = ctx->strm_.next_in;
        }

        switch (ctx->gzip_id_bytes_read_) {
          case 0:
            if (next_expected_header_byte == nullptr) {
              break;
            }

            if (*next_expected_header_byte == GZIP_HEADER_ID1) {
              ctx->gzip_id_bytes_read_ = 1;
              next_expected_header_byte++;

              if (ctx->strm_.avail_in == 1) {
                // The only available byte was already read.
                break;
              }
            } else {
              ctx->mode_ = INFLATE;
              break;
            }

            // fallthrough
          case 1:
            if (next_expected_header_byte == nullptr) {
              break;
            }

            if (*next_expected_header_byte == GZIP_HEADER_ID2) {
              ctx->gzip_id_bytes_read_ = 2;
              ctx->mode_ = GUNZIP;
            } else {
              // There is no actual difference between INFLATE and INFLATERAW
              // (after initialization).
              ctx->mode_ = INFLATE;
            }

            break;
          default:
            CHECK(0 && "invalid number of gzip magic number bytes read");
        }

        // fallthrough
      case INFLATE:
      case GUNZIP:
      case INFLATERAW:
        ctx->err_ = inflate(&ctx->strm_, ctx->flush_);

        // If data was encoded with dictionary
        if (ctx->err_ == Z_NEED_DICT && ctx->dictionary_ != nullptr) {
          // Load it
          ctx->err_ = inflateSetDictionary(&ctx->strm_,
                                           ctx->dictionary_,
                                           ctx->dictionary_len_);
          if (ctx->err_ == Z_OK) {
            // And try to decode again
            ctx->err_ = inflate(&ctx->strm_, ctx->flush_);
          } else if (ctx->err_ == Z_DATA_ERROR) {
            // Both inflateSetDictionary() and inflate() return Z_DATA_ERROR.
            // Make it possible for After() to tell a bad dictionary from bad
            // input.
            ctx->err_ = Z_NEED_DICT;
          }
        }

        while (ctx->strm_.avail_in > 0 &&
               ctx->mode_ == GUNZIP &&
               ctx->err_ == Z_STREAM_END &&
               ctx->strm_.next_in[0] != 0x00 &&
               (!ctx->gzip_header_ || !ctx->gzip_header_->hasBeenRead())) {
          // Bytes remain in input buffer. Perhaps this is another compressed
          // member in the same archive, or just trailing garbage.
          // Trailing zero bytes are okay, though, since they are frequently
          // used for padding.

          Reset(ctx);
          SetGZipHeaderBuffer(ctx);

          ctx->err_ = inflate(&ctx->strm_, ctx->flush_);
        }
        break;
      default:
        CHECK(0 && "wtf?");
    }

    // pass any errors back to the main thread to deal with.

    // now After will emit the output, and
    // either schedule another call to Process,
    // or shift the queue and call Process.
  }


  static bool CheckError(ZCtx* ctx) {
    // Acceptable error states depend on the type of zlib stream.
    switch (ctx->err_) {
    case Z_OK:
    case Z_BUF_ERROR:
      if (ctx->strm_.avail_out != 0 && ctx->flush_ == Z_FINISH) {
        ZCtx::Error(ctx, "unexpected end of file");
        return false;
      }
    case Z_STREAM_END:
      // normal statuses, not fatal
      break;
    case Z_NEED_DICT:
      if (ctx->dictionary_ == nullptr)
        ZCtx::Error(ctx, "Missing dictionary");
      else
        ZCtx::Error(ctx, "Bad dictionary");
      return false;
    default:
      // something else.
      ZCtx::Error(ctx, "Zlib error");
      return false;
    }

    return true;
  }


  // v8 land!
  static void After(uv_work_t* work_req, int status) {
    CHECK_EQ(status, 0);

    ZCtx* ctx = ContainerOf(&ZCtx::work_req_, work_req);
    Environment* env = ctx->env();

    HandleScope handle_scope(env->isolate());
    Context::Scope context_scope(env->context());

    if (!CheckError(ctx))
      return;

    Local<Integer> avail_out = Integer::New(env->isolate(),
                                            ctx->strm_.avail_out);
    Local<Integer> avail_in = Integer::New(env->isolate(),
                                           ctx->strm_.avail_in);

    Local<Value> gzip_header;
    if (ctx->gzip_header_ && ctx->gzip_header_->hasBeenRead()) {
      gzip_header = ctx->gzip_header_->toObject(env);
      ctx->gzip_header_->resetHasBeenRead();
    } else {
      gzip_header = Null(env->isolate());
    }

    ctx->write_in_progress_ = false;

    // call the write() cb
    Local<Value> args[3] = { avail_in, avail_out, gzip_header };
    ctx->MakeCallback(env->callback_string(), arraysize(args), args);

    ctx->Unref();
    if (ctx->pending_close_)
      ctx->Close();
  }

  static void Error(ZCtx* ctx, const char* message) {
    Environment* env = ctx->env();

    // If you hit this assertion, you forgot to enter the v8::Context first.
    CHECK_EQ(env->context(), env->isolate()->GetCurrentContext());

    if (ctx->strm_.msg != nullptr) {
      message = ctx->strm_.msg;
    }

    HandleScope scope(env->isolate());
    Local<Value> args[2] = {
      OneByteString(env->isolate(), message),
      Number::New(env->isolate(), ctx->err_)
    };
    ctx->MakeCallback(env->onerror_string(), arraysize(args), args);

    // no hope of rescue.
    if (ctx->write_in_progress_)
      ctx->Unref();
    ctx->write_in_progress_ = false;
    if (ctx->pending_close_)
      ctx->Close();
  }

  static void New(const FunctionCallbackInfo<Value>& args) {
    Environment* env = Environment::GetCurrent(args);

    if (args.Length() < 1 || !args[0]->IsInt32()) {
      return env->ThrowTypeError("Bad argument");
    }
    node_zlib_mode mode = static_cast<node_zlib_mode>(args[0]->Int32Value());

    if (mode < DEFLATE || mode > UNZIP) {
      return env->ThrowTypeError("Bad argument");
    }

    new ZCtx(env, args.This(), mode);
  }

  // just pull the ints out of the args and call the other Init
  static void Init(const FunctionCallbackInfo<Value>& args) {
    CHECK((args.Length() >= 4 && args.Length() <= 6) &&
      "init(windowBits, level, memLevel, strategy, [dictionary, gzipHeader])");

    ZCtx* ctx = Unwrap<ZCtx>(args.Holder());

    int windowBits = args[0]->Uint32Value();
    CHECK((windowBits >= 8 && windowBits <= 15) && "invalid windowBits");

    int level = args[1]->Int32Value();
    CHECK((level >= -1 && level <= 9) && "invalid compression level");

    int memLevel = args[2]->Uint32Value();
    CHECK((memLevel >= 1 && memLevel <= 9) && "invalid memlevel");

    int strategy = args[3]->Uint32Value();
    CHECK((strategy == Z_FILTERED ||
            strategy == Z_HUFFMAN_ONLY ||
            strategy == Z_RLE ||
            strategy == Z_FIXED ||
            strategy == Z_DEFAULT_STRATEGY) && "invalid strategy");

    char* dictionary = nullptr;
    size_t dictionary_len = 0;
    if (args.Length() >= 5 && Buffer::HasInstance(args[4])) {
      Local<Object> dictionary_ = args[4]->ToObject(args.GetIsolate());

      dictionary_len = Buffer::Length(dictionary_);
      dictionary = new char[dictionary_len];

      memcpy(dictionary, Buffer::Data(dictionary_), dictionary_len);
    }

    GZipHeader* gzip_header = nullptr;
    if (args.Length() >= 6) {
      if ((ctx->mode_ == GUNZIP || ctx->mode_ == UNZIP) &&
          args[5]->BooleanValue()) {
        gzip_header = new GZipHeader(ctx->env());
      } else if (ctx->mode_ == GZIP && args[5]->IsObject()) {
        gzip_header = new GZipHeader(ctx->env(), args[5]->ToObject());
      }
    }

    Init(ctx, level, windowBits, memLevel, strategy,
         dictionary, dictionary_len, gzip_header);
    SetDictionary(ctx);
  }

  static void Params(const FunctionCallbackInfo<Value>& args) {
    CHECK(args.Length() == 2 && "params(level, strategy)");
    ZCtx* ctx = Unwrap<ZCtx>(args.Holder());
    Params(ctx, args[0]->Int32Value(), args[1]->Int32Value());
  }

  static void Reset(const FunctionCallbackInfo<Value> &args) {
    ZCtx* ctx = Unwrap<ZCtx>(args.Holder());
    Reset(ctx);
    SetDictionary(ctx);
  }

  static void Init(ZCtx *ctx, int level, int windowBits, int memLevel,
                   int strategy, char* dictionary, size_t dictionary_len,
                   GZipHeader* gzip_header) {
    ctx->level_ = level;
    ctx->windowBits_ = windowBits;
    ctx->memLevel_ = memLevel;
    ctx->strategy_ = strategy;
    ctx->gzip_header_ = gzip_header;

    ctx->strm_.zalloc = Z_NULL;
    ctx->strm_.zfree = Z_NULL;
    ctx->strm_.opaque = Z_NULL;

    ctx->flush_ = Z_NO_FLUSH;

    ctx->err_ = Z_OK;

    if (ctx->mode_ == GZIP || ctx->mode_ == GUNZIP) {
      ctx->windowBits_ += 16;
    }

    if (ctx->mode_ == UNZIP) {
      ctx->windowBits_ += 32;
    }

    if (ctx->mode_ == DEFLATERAW || ctx->mode_ == INFLATERAW) {
      ctx->windowBits_ *= -1;
    }

    switch (ctx->mode_) {
      case DEFLATE:
      case GZIP:
      case DEFLATERAW:
        ctx->err_ = deflateInit2(&ctx->strm_,
                                 ctx->level_,
                                 Z_DEFLATED,
                                 ctx->windowBits_,
                                 ctx->memLevel_,
                                 ctx->strategy_);
        ctx->env()->isolate()
            ->AdjustAmountOfExternalAllocatedMemory(kDeflateContextSize);
        break;
      case INFLATE:
      case GUNZIP:
      case INFLATERAW:
      case UNZIP:
        ctx->err_ = inflateInit2(&ctx->strm_, ctx->windowBits_);
        ctx->env()->isolate()
            ->AdjustAmountOfExternalAllocatedMemory(kInflateContextSize);
        break;
      default:
        CHECK(0 && "wtf?");
    }

    if (ctx->err_ != Z_OK) {
      ZCtx::Error(ctx, "Init error");
    } else {
      SetGZipHeaderBuffer(ctx);
    }

    ctx->dictionary_ = reinterpret_cast<Bytef *>(dictionary);
    ctx->dictionary_len_ = dictionary_len;

    ctx->write_in_progress_ = false;
    ctx->init_done_ = true;
  }

  static void SetGZipHeaderBuffer(ZCtx* ctx) {
    if (!ctx->gzip_header_) {
      return;
    }

    if (ctx->mode_ == GZIP) {
      ctx->err_ = deflateSetHeader(&ctx->strm_,
                                   ctx->gzip_header_->underlying());
    } else {
      ctx->gzip_header_->resetHasBeenRead();
      ctx->err_ = inflateGetHeader(&ctx->strm_,
                                   ctx->gzip_header_->underlying());
    }

    if (ctx->err_ != Z_OK) {
      ZCtx::Error(ctx, "Error during setting up buffer for header fields");
    }
  }

  static void SetDictionary(ZCtx* ctx) {
    if (ctx->dictionary_ == nullptr)
      return;

    ctx->err_ = Z_OK;

    switch (ctx->mode_) {
      case DEFLATE:
      case DEFLATERAW:
        ctx->err_ = deflateSetDictionary(&ctx->strm_,
                                         ctx->dictionary_,
                                         ctx->dictionary_len_);
        break;
      default:
        break;
    }

    if (ctx->err_ != Z_OK) {
      ZCtx::Error(ctx, "Failed to set dictionary");
    }
  }

  static void Params(ZCtx* ctx, int level, int strategy) {
    ctx->err_ = Z_OK;

    switch (ctx->mode_) {
      case DEFLATE:
      case DEFLATERAW:
        ctx->err_ = deflateParams(&ctx->strm_, level, strategy);
        break;
      default:
        break;
    }

    if (ctx->err_ != Z_OK && ctx->err_ != Z_BUF_ERROR) {
      ZCtx::Error(ctx, "Failed to set parameters");
    }
  }

  static void Reset(ZCtx* ctx) {
    ctx->err_ = Z_OK;

    switch (ctx->mode_) {
      case DEFLATE:
      case DEFLATERAW:
      case GZIP:
        ctx->err_ = deflateReset(&ctx->strm_);
        break;
      case INFLATE:
      case INFLATERAW:
      case GUNZIP:
        ctx->err_ = inflateReset(&ctx->strm_);
        break;
      default:
        break;
    }

    if (ctx->err_ != Z_OK) {
      ZCtx::Error(ctx, "Failed to reset stream");
    }
  }

  size_t self_size() const override { return sizeof(*this); }

 private:
  void Ref() {
    if (++refs_ == 1) {
      ClearWeak();
    }
  }

  void Unref() {
    CHECK_GT(refs_, 0);
    if (--refs_ == 0) {
      MakeWeak<ZCtx>(this);
    }
  }

  static const int kDeflateContextSize = 16384;  // approximate
  static const int kInflateContextSize = 10240;  // approximate

  int chunk_size_;
  Bytef* dictionary_;
  size_t dictionary_len_;
  int err_;
  int flush_;
  bool init_done_;
  int level_;
  int memLevel_;
  node_zlib_mode mode_;
  int strategy_;
  z_stream strm_;
  int windowBits_;
  uv_work_t work_req_;
  GZipHeader* gzip_header_;
  bool write_in_progress_;
  bool pending_close_;
  unsigned int refs_;
  unsigned int gzip_id_bytes_read_;
};


void InitZlib(Local<Object> target,
              Local<Value> unused,
              Local<Context> context,
              void* priv) {
  Environment* env = Environment::GetCurrent(context);
  Local<FunctionTemplate> z = env->NewFunctionTemplate(ZCtx::New);

  z->InstanceTemplate()->SetInternalFieldCount(1);

  env->SetProtoMethod(z, "write", ZCtx::Write<true>);
  env->SetProtoMethod(z, "writeSync", ZCtx::Write<false>);
  env->SetProtoMethod(z, "init", ZCtx::Init);
  env->SetProtoMethod(z, "close", ZCtx::Close);
  env->SetProtoMethod(z, "params", ZCtx::Params);
  env->SetProtoMethod(z, "reset", ZCtx::Reset);

  z->SetClassName(FIXED_ONE_BYTE_STRING(env->isolate(), "Zlib"));
  target->Set(FIXED_ONE_BYTE_STRING(env->isolate(), "Zlib"), z->GetFunction());

  // valid flush values.
  NODE_DEFINE_CONSTANT(target, Z_NO_FLUSH);
  NODE_DEFINE_CONSTANT(target, Z_PARTIAL_FLUSH);
  NODE_DEFINE_CONSTANT(target, Z_SYNC_FLUSH);
  NODE_DEFINE_CONSTANT(target, Z_FULL_FLUSH);
  NODE_DEFINE_CONSTANT(target, Z_FINISH);
  NODE_DEFINE_CONSTANT(target, Z_BLOCK);

  // return/error codes
  NODE_DEFINE_CONSTANT(target, Z_OK);
  NODE_DEFINE_CONSTANT(target, Z_STREAM_END);
  NODE_DEFINE_CONSTANT(target, Z_NEED_DICT);
  NODE_DEFINE_CONSTANT(target, Z_ERRNO);
  NODE_DEFINE_CONSTANT(target, Z_STREAM_ERROR);
  NODE_DEFINE_CONSTANT(target, Z_DATA_ERROR);
  NODE_DEFINE_CONSTANT(target, Z_MEM_ERROR);
  NODE_DEFINE_CONSTANT(target, Z_BUF_ERROR);
  NODE_DEFINE_CONSTANT(target, Z_VERSION_ERROR);

  NODE_DEFINE_CONSTANT(target, Z_NO_COMPRESSION);
  NODE_DEFINE_CONSTANT(target, Z_BEST_SPEED);
  NODE_DEFINE_CONSTANT(target, Z_BEST_COMPRESSION);
  NODE_DEFINE_CONSTANT(target, Z_DEFAULT_COMPRESSION);
  NODE_DEFINE_CONSTANT(target, Z_FILTERED);
  NODE_DEFINE_CONSTANT(target, Z_HUFFMAN_ONLY);
  NODE_DEFINE_CONSTANT(target, Z_RLE);
  NODE_DEFINE_CONSTANT(target, Z_FIXED);
  NODE_DEFINE_CONSTANT(target, Z_DEFAULT_STRATEGY);
  NODE_DEFINE_CONSTANT(target, ZLIB_VERNUM);

  NODE_DEFINE_CONSTANT(target, DEFLATE);
  NODE_DEFINE_CONSTANT(target, INFLATE);
  NODE_DEFINE_CONSTANT(target, GZIP);
  NODE_DEFINE_CONSTANT(target, GUNZIP);
  NODE_DEFINE_CONSTANT(target, DEFLATERAW);
  NODE_DEFINE_CONSTANT(target, INFLATERAW);
  NODE_DEFINE_CONSTANT(target, UNZIP);

  target->Set(FIXED_ONE_BYTE_STRING(env->isolate(), "ZLIB_VERSION"),
              FIXED_ONE_BYTE_STRING(env->isolate(), ZLIB_VERSION));
}

}  // namespace node

NODE_MODULE_CONTEXT_AWARE_BUILTIN(zlib, node::InitZlib)
