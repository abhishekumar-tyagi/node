#include "node_blob.h"
#include "async_wrap-inl.h"
#include "base_object-inl.h"
#include "env-inl.h"
#include "memory_tracker-inl.h"
#include "node_errors.h"
#include "node_external_reference.h"
#include "threadpoolwork-inl.h"
#include "v8.h"

#include <algorithm>

namespace node {

using v8::Array;
using v8::ArrayBuffer;
using v8::ArrayBufferView;
using v8::BackingStore;
using v8::Context;
using v8::EscapableHandleScope;
using v8::Function;
using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::HandleScope;
using v8::Local;
using v8::MaybeLocal;
using v8::Number;
using v8::Object;
using v8::Uint32;
using v8::Undefined;
using v8::Value;

void Blob::Initialize(Environment* env, v8::Local<v8::Object> target) {
  env->SetMethod(target, "createBlob", New);
  FixedSizeBlobCopyJob::Initialize(env, target);
}

Local<FunctionTemplate> Blob::GetConstructorTemplate(Environment* env) {
  Local<FunctionTemplate> tmpl = env->blob_constructor_template();
  if (tmpl.IsEmpty()) {
    tmpl = FunctionTemplate::New(env->isolate());
    tmpl->InstanceTemplate()->SetInternalFieldCount(
        BaseObject::kInternalFieldCount);
    tmpl->Inherit(BaseObject::GetConstructorTemplate(env));
    tmpl->SetClassName(
        FIXED_ONE_BYTE_STRING(env->isolate(), "Blob"));
    env->SetProtoMethod(tmpl, "toArrayBuffer", ToArrayBuffer);
    env->SetProtoMethod(tmpl, "slice", ToSlice);
    env->set_blob_constructor_template(tmpl);
  }
  return tmpl;
}

bool Blob::HasInstance(Environment* env, v8::Local<v8::Value> object) {
  return GetConstructorTemplate(env)->HasInstance(object);
}

BaseObjectPtr<Blob> Blob::Create(
    Environment* env,
    const std::vector<BlobEntry> store,
    size_t length) {

  HandleScope scope(env->isolate());

  Local<Function> ctor;
  if (!GetConstructorTemplate(env)->GetFunction(env->context()).ToLocal(&ctor))
    return BaseObjectPtr<Blob>();

  Local<Object> obj;
  if (!ctor->NewInstance(env->context()).ToLocal(&obj))
    return BaseObjectPtr<Blob>();

  return MakeBaseObject<Blob>(env, obj, store, length);
}

void Blob::New(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  CHECK(args[0]->IsArray());  // sources
  CHECK(args[1]->IsUint32());  // length

  std::vector<BlobEntry> entries;

  size_t length = args[1].As<Uint32>()->Value();
  size_t len = 0;
  Local<Array> ary = args[0].As<Array>();
  for (size_t n = 0; n < ary->Length(); n++) {
    Local<Value> entry;
    if (!ary->Get(env->context(), n).ToLocal(&entry))
      return;
    CHECK(entry->IsArrayBufferView() || Blob::HasInstance(env, entry));
    if (entry->IsArrayBufferView()) {
      Local<ArrayBufferView> view = entry.As<ArrayBufferView>();
      CHECK_EQ(view->ByteOffset(), 0);
      std::shared_ptr<BackingStore> store = view->Buffer()->GetBackingStore();
      size_t byte_length = view->ByteLength();
      view->Buffer()->Detach();  // The Blob will own the backing store now.
      entries.emplace_back(BlobEntry{std::move(store), byte_length, 0});
      len += byte_length;
    } else {
      Blob* blob;
      ASSIGN_OR_RETURN_UNWRAP(&blob, entry);
      auto source = blob->entries();
      entries.insert(entries.end(), source.begin(), source.end());
      len += blob->length();
    }
  }
  CHECK_EQ(length, len);

  BaseObjectPtr<Blob> blob = Create(env, entries, length);
  if (blob)
    args.GetReturnValue().Set(blob->object());
}

void Blob::ToArrayBuffer(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  Blob* blob;
  ASSIGN_OR_RETURN_UNWRAP(&blob, args.Holder());
  Local<Value> ret;
  if (blob->GetArrayBuffer(env).ToLocal(&ret))
    args.GetReturnValue().Set(ret);
}

void Blob::ToSlice(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  Blob* blob;
  ASSIGN_OR_RETURN_UNWRAP(&blob, args.Holder());
  CHECK(args[0]->IsUint32());
  CHECK(args[1]->IsUint32());
  size_t start = args[0].As<Uint32>()->Value();
  size_t end = args[1].As<Uint32>()->Value();
  BaseObjectPtr<Blob> slice = blob->Slice(env, start, end);
  if (slice)
    args.GetReturnValue().Set(slice->object());
}

void Blob::MemoryInfo(MemoryTracker* tracker) const {
  tracker->TrackFieldWithSize("store", length_);
}

MaybeLocal<Value> Blob::GetArrayBuffer(Environment* env) {
  EscapableHandleScope scope(env->isolate());
  size_t len = length();
  std::shared_ptr<BackingStore> store =
      ArrayBuffer::NewBackingStore(env->isolate(), len);
  if (len > 0) {
    unsigned char* dest = static_cast<unsigned char*>(store->Data());
    size_t total = 0;
    for (const auto& entry : entries()) {
      unsigned char* src = static_cast<unsigned char*>(entry.store->Data());
      src += entry.offset;
      memcpy(dest, src, entry.length);
      dest += entry.length;
      total += entry.length;
      CHECK_LE(total, len);
    }
  }

  return scope.Escape(ArrayBuffer::New(env->isolate(), store));
}

BaseObjectPtr<Blob> Blob::Slice(Environment* env, size_t start, size_t end) {
  CHECK_LE(start, length());
  CHECK_LE(end, length());
  CHECK_LE(start, end);

  std::vector<BlobEntry> slices;
  size_t total = end - start;
  size_t remaining = total;

  if (total == 0) return Create(env, slices, 0);

  for (const auto& entry : entries()) {
    if (start + entry.offset > entry.store->ByteLength()) {
      start -= entry.length;
      continue;
    }

    size_t offset = entry.offset + start;
    size_t len = std::min(remaining, entry.store->ByteLength() - offset);
    slices.emplace_back(BlobEntry{entry.store, len, offset});

    remaining -= len;
    start = 0;

    if (remaining == 0)
      break;
  }

  return Create(env, slices, total);
}

Blob::Blob(
    Environment* env,
    v8::Local<v8::Object> obj,
    const std::vector<BlobEntry>& store,
    size_t length)
    : BaseObject(env, obj),
      store_(store),
      length_(length) {
  MakeWeak();
}

BaseObjectPtr<BaseObject>
Blob::BlobTransferData::Deserialize(
    Environment* env,
    Local<Context> context,
    std::unique_ptr<worker::TransferData> self) {
  if (context != env->context()) {
    THROW_ERR_MESSAGE_TARGET_CONTEXT_UNAVAILABLE(env);
    return {};
  }
  return Blob::Create(env, store_, length_);
}

BaseObject::TransferMode Blob::GetTransferMode() const {
  return BaseObject::TransferMode::kCloneable;
}

std::unique_ptr<worker::TransferData> Blob::CloneForMessaging() const {
  return std::make_unique<BlobTransferData>(store_, length_);
}

FixedSizeBlobCopyJob::FixedSizeBlobCopyJob(
    Environment* env,
    Local<Object> object,
    Blob* blob,
    FixedSizeBlobCopyJob::Mode mode)
    : AsyncWrap(env, object, AsyncWrap::PROVIDER_FIXEDSIZEBLOBCOPY),
      ThreadPoolWork(env),
      mode_(mode) {
  if (mode == FixedSizeBlobCopyJob::Mode::SYNC) MakeWeak();
  source_ = blob->entries();
  length_ = blob->length();
}

void FixedSizeBlobCopyJob::AfterThreadPoolWork(int status) {
  Environment* env = AsyncWrap::env();
  CHECK_EQ(mode_, Mode::ASYNC);
  CHECK(status == 0 || status == UV_ECANCELED);
  std::unique_ptr<FixedSizeBlobCopyJob> ptr(this);
  HandleScope handle_scope(env->isolate());
  Context::Scope context_scope(env->context());
  Local<Value> args[2];

  if (status == UV_ECANCELED) {
    args[0] = Number::New(env->isolate(), status),
    args[1] = Undefined(env->isolate());
  } else {
    args[0] = Undefined(env->isolate());
    args[1] = ArrayBuffer::New(env->isolate(), destination_);
  }

  ptr->MakeCallback(env->ondone_string(), arraysize(args), args);
}

void FixedSizeBlobCopyJob::DoThreadPoolWork() {
  unsigned char* dest = static_cast<unsigned char*>(destination_->Data());
  if (length_ > 0) {
    size_t total = 0;
    for (const auto& entry : source_) {
      unsigned char* src = static_cast<unsigned char*>(entry.store->Data());
      src += entry.offset;
      memcpy(dest, src, entry.length);
      dest += entry.length;
      total += entry.length;
      CHECK_LE(total, length_);
    }
  }
}

void FixedSizeBlobCopyJob::MemoryInfo(MemoryTracker* tracker) const {
  tracker->TrackFieldWithSize("source", length_);
  tracker->TrackFieldWithSize(
      "destination",
      destination_ ? destination_->ByteLength() : 0);
}

void FixedSizeBlobCopyJob::Initialize(Environment* env, Local<Object> target) {
  v8::Local<v8::FunctionTemplate> job = env->NewFunctionTemplate(New);
  job->Inherit(AsyncWrap::GetConstructorTemplate(env));
  job->InstanceTemplate()->SetInternalFieldCount(
      AsyncWrap::kInternalFieldCount);
  env->SetProtoMethod(job, "run", Run);
  env->SetConstructorFunction(target, "FixedSizeBlobCopyJob", job);
}

void FixedSizeBlobCopyJob::New(const FunctionCallbackInfo<Value>& args) {
  static constexpr size_t kMaxSyncLength = 4096;
  static constexpr size_t kMaxEntryCount = 4;

  Environment* env = Environment::GetCurrent(args);
  CHECK(args.IsConstructCall());
  CHECK(args[0]->IsObject());
  CHECK(Blob::HasInstance(env, args[0]));

  Blob* blob;
  ASSIGN_OR_RETURN_UNWRAP(&blob, args[0]);

  // This is a fairly arbitrary heuristic. We want to avoid deferring to
  // the threadpool if the amount of data being copied is small and there
  // aren't that many entries to copy.
  FixedSizeBlobCopyJob::Mode mode =
      (blob->length() < kMaxSyncLength &&
       blob->entries().size() < kMaxEntryCount) ?
          FixedSizeBlobCopyJob::Mode::SYNC :
          FixedSizeBlobCopyJob::Mode::ASYNC;

  new FixedSizeBlobCopyJob(env, args.This(), blob, mode);
}

void FixedSizeBlobCopyJob::Run(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  FixedSizeBlobCopyJob* job;
  ASSIGN_OR_RETURN_UNWRAP(&job, args.Holder());
  job->destination_ =
      ArrayBuffer::NewBackingStore(env->isolate(), job->length_);
  if (job->mode() == FixedSizeBlobCopyJob::Mode::ASYNC)
    return job->ScheduleWork();

  job->DoThreadPoolWork();
  args.GetReturnValue().Set(
      ArrayBuffer::New(env->isolate(), job->destination_));
}

void FixedSizeBlobCopyJob::RegisterExternalReferences(
    ExternalReferenceRegistry* registry) {
  registry->Register(New);
  registry->Register(Run);
}

void Blob::RegisterExternalReferences(ExternalReferenceRegistry* registry) {
  registry->Register(Blob::New);
  registry->Register(Blob::ToArrayBuffer);
  registry->Register(Blob::ToSlice);
}

}  // namespace node
