// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: google/protobuf/any.proto

#include <google/protobuf/any.pb.h>

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/wire_format_lite.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
PROTOBUF_NAMESPACE_OPEN
class AnyDefaultTypeInternal {
 public:
  ::PROTOBUF_NAMESPACE_ID::internal::ExplicitlyConstructed<Any> _instance;
} _Any_default_instance_;
PROTOBUF_NAMESPACE_CLOSE
static void InitDefaultsscc_info_Any_google_2fprotobuf_2fany_2eproto() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &PROTOBUF_NAMESPACE_ID::_Any_default_instance_;
    new (ptr) PROTOBUF_NAMESPACE_ID::Any();
    ::PROTOBUF_NAMESPACE_ID::internal::OnShutdownDestroyMessage(ptr);
  }
  PROTOBUF_NAMESPACE_ID::Any::InitAsDefaultInstance();
}

PROTOBUF_EXPORT ::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<0> scc_info_Any_google_2fprotobuf_2fany_2eproto =
    {{ATOMIC_VAR_INIT(::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase::kUninitialized), 0, InitDefaultsscc_info_Any_google_2fprotobuf_2fany_2eproto}, {}};

static ::PROTOBUF_NAMESPACE_ID::Metadata file_level_metadata_google_2fprotobuf_2fany_2eproto[1];
static constexpr ::PROTOBUF_NAMESPACE_ID::EnumDescriptor const** file_level_enum_descriptors_google_2fprotobuf_2fany_2eproto = nullptr;
static constexpr ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor const** file_level_service_descriptors_google_2fprotobuf_2fany_2eproto = nullptr;

const ::PROTOBUF_NAMESPACE_ID::uint32 TableStruct_google_2fprotobuf_2fany_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(PROTOBUF_NAMESPACE_ID::Any, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(PROTOBUF_NAMESPACE_ID::Any, type_url_),
  PROTOBUF_FIELD_OFFSET(PROTOBUF_NAMESPACE_ID::Any, value_),
};
static const ::PROTOBUF_NAMESPACE_ID::internal::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, sizeof(PROTOBUF_NAMESPACE_ID::Any)},
};

static ::PROTOBUF_NAMESPACE_ID::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&PROTOBUF_NAMESPACE_ID::_Any_default_instance_),
};

const char descriptor_table_protodef_google_2fprotobuf_2fany_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n\031google/protobuf/any.proto\022\017google.prot"
  "obuf\"&\n\003Any\022\020\n\010type_url\030\001 \001(\t\022\r\n\005value\030\002"
  " \001(\014Bo\n\023com.google.protobufB\010AnyProtoP\001Z"
  "%github.com/golang/protobuf/ptypes/any\242\002"
  "\003GPB\252\002\036Google.Protobuf.WellKnownTypesb\006p"
  "roto3"
  ;
static const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable*const descriptor_table_google_2fprotobuf_2fany_2eproto_deps[1] = {
};
static ::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase*const descriptor_table_google_2fprotobuf_2fany_2eproto_sccs[1] = {
  &scc_info_Any_google_2fprotobuf_2fany_2eproto.base,
};
static ::PROTOBUF_NAMESPACE_ID::internal::once_flag descriptor_table_google_2fprotobuf_2fany_2eproto_once;
static bool descriptor_table_google_2fprotobuf_2fany_2eproto_initialized = false;
const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_google_2fprotobuf_2fany_2eproto = {
  &descriptor_table_google_2fprotobuf_2fany_2eproto_initialized, descriptor_table_protodef_google_2fprotobuf_2fany_2eproto, "google/protobuf/any.proto", 205,
  &descriptor_table_google_2fprotobuf_2fany_2eproto_once, descriptor_table_google_2fprotobuf_2fany_2eproto_sccs, descriptor_table_google_2fprotobuf_2fany_2eproto_deps, 1, 0,
  schemas, file_default_instances, TableStruct_google_2fprotobuf_2fany_2eproto::offsets,
  file_level_metadata_google_2fprotobuf_2fany_2eproto, 1, file_level_enum_descriptors_google_2fprotobuf_2fany_2eproto, file_level_service_descriptors_google_2fprotobuf_2fany_2eproto,
};

// Force running AddDescriptors() at dynamic initialization time.
static bool dynamic_init_dummy_google_2fprotobuf_2fany_2eproto = (  ::PROTOBUF_NAMESPACE_ID::internal::AddDescriptors(&descriptor_table_google_2fprotobuf_2fany_2eproto), true);
PROTOBUF_NAMESPACE_OPEN

// ===================================================================

void Any::InitAsDefaultInstance() {
}
void Any::PackFrom(const ::PROTOBUF_NAMESPACE_ID::Message& message) {
  _any_metadata_.PackFrom(message);
}

void Any::PackFrom(const ::PROTOBUF_NAMESPACE_ID::Message& message,
                           const std::string& type_url_prefix) {
  _any_metadata_.PackFrom(message, type_url_prefix);
}

bool Any::UnpackTo(::PROTOBUF_NAMESPACE_ID::Message* message) const {
  return _any_metadata_.UnpackTo(message);
}
bool Any::GetAnyFieldDescriptors(
    const ::PROTOBUF_NAMESPACE_ID::Message& message,
    const ::PROTOBUF_NAMESPACE_ID::FieldDescriptor** type_url_field,
    const ::PROTOBUF_NAMESPACE_ID::FieldDescriptor** value_field) {
  return ::PROTOBUF_NAMESPACE_ID::internal::GetAnyFieldDescriptors(
      message, type_url_field, value_field);
}
bool Any::ParseAnyTypeUrl(const string& type_url,
                                  std::string* full_type_name) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseAnyTypeUrl(type_url,
                                             full_type_name);
}

class Any::_Internal {
 public:
};

Any::Any()
  : ::PROTOBUF_NAMESPACE_ID::Message(), _internal_metadata_(nullptr), _any_metadata_(&type_url_, &value_) {
  SharedCtor();
  // @@protoc_insertion_point(constructor:google.protobuf.Any)
}
Any::Any(const Any& from)
  : ::PROTOBUF_NAMESPACE_ID::Message(),
      _internal_metadata_(nullptr),
      _any_metadata_(&type_url_, &value_) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  type_url_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (!from.type_url().empty()) {
    type_url_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.type_url_);
  }
  value_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (!from.value().empty()) {
    value_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.value_);
  }
  // @@protoc_insertion_point(copy_constructor:google.protobuf.Any)
}

void Any::SharedCtor() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&scc_info_Any_google_2fprotobuf_2fany_2eproto.base);
  type_url_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  value_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

Any::~Any() {
  // @@protoc_insertion_point(destructor:google.protobuf.Any)
  SharedDtor();
}

void Any::SharedDtor() {
  type_url_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  value_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

void Any::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const Any& Any::default_instance() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&::scc_info_Any_google_2fprotobuf_2fany_2eproto.base);
  return *internal_default_instance();
}


void Any::Clear() {
// @@protoc_insertion_point(message_clear_start:google.protobuf.Any)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  type_url_.ClearToEmptyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  value_.ClearToEmptyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  _internal_metadata_.Clear();
}

#if GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
const char* Any::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {
      // string type_url = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 10)) {
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParserUTF8(mutable_type_url(), ptr, ctx, "google.protobuf.Any.type_url");
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // bytes value = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 18)) {
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(mutable_value(), ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag, &_internal_metadata_, ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }  // switch
  }  // while
success:
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}
#else  // GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
bool Any::MergePartialFromCodedStream(
    ::PROTOBUF_NAMESPACE_ID::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!PROTOBUF_PREDICT_TRUE(EXPRESSION)) goto failure
  ::PROTOBUF_NAMESPACE_ID::uint32 tag;
  // @@protoc_insertion_point(parse_start:google.protobuf.Any)
  for (;;) {
    ::std::pair<::PROTOBUF_NAMESPACE_ID::uint32, bool> p = input->ReadTagWithCutoffNoLastTag(127u);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // string type_url = 1;
      case 1: {
        if (static_cast< ::PROTOBUF_NAMESPACE_ID::uint8>(tag) == (10 & 0xFF)) {
          DO_(::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::ReadString(
                input, this->mutable_type_url()));
          DO_(::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
            this->type_url().data(), static_cast<int>(this->type_url().length()),
            ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::PARSE,
            "google.protobuf.Any.type_url"));
        } else {
          goto handle_unusual;
        }
        break;
      }

      // bytes value = 2;
      case 2: {
        if (static_cast< ::PROTOBUF_NAMESPACE_ID::uint8>(tag) == (18 & 0xFF)) {
          DO_(::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::ReadBytes(
                input, this->mutable_value()));
        } else {
          goto handle_unusual;
        }
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0) {
          goto success;
        }
        DO_(::PROTOBUF_NAMESPACE_ID::internal::WireFormat::SkipField(
              input, tag, _internal_metadata_.mutable_unknown_fields()));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:google.protobuf.Any)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:google.protobuf.Any)
  return false;
#undef DO_
}
#endif  // GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER

void Any::SerializeWithCachedSizes(
    ::PROTOBUF_NAMESPACE_ID::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:google.protobuf.Any)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // string type_url = 1;
  if (this->type_url().size() > 0) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->type_url().data(), static_cast<int>(this->type_url().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "google.protobuf.Any.type_url");
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteStringMaybeAliased(
      1, this->type_url(), output);
  }

  // bytes value = 2;
  if (this->value().size() > 0) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBytesMaybeAliased(
      2, this->value(), output);
  }

  if (_internal_metadata_.have_unknown_fields()) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::SerializeUnknownFields(
        _internal_metadata_.unknown_fields(), output);
  }
  // @@protoc_insertion_point(serialize_end:google.protobuf.Any)
}

::PROTOBUF_NAMESPACE_ID::uint8* Any::InternalSerializeWithCachedSizesToArray(
    ::PROTOBUF_NAMESPACE_ID::uint8* target) const {
  // @@protoc_insertion_point(serialize_to_array_start:google.protobuf.Any)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // string type_url = 1;
  if (this->type_url().size() > 0) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->type_url().data(), static_cast<int>(this->type_url().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "google.protobuf.Any.type_url");
    target =
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteStringToArray(
        1, this->type_url(), target);
  }

  // bytes value = 2;
  if (this->value().size() > 0) {
    target =
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBytesToArray(
        2, this->value(), target);
  }

  if (_internal_metadata_.have_unknown_fields()) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::SerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields(), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:google.protobuf.Any)
  return target;
}

size_t Any::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:google.protobuf.Any)
  size_t total_size = 0;

  if (_internal_metadata_.have_unknown_fields()) {
    total_size +=
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::ComputeUnknownFieldsSize(
        _internal_metadata_.unknown_fields());
  }
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // string type_url = 1;
  if (this->type_url().size() > 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->type_url());
  }

  // bytes value = 2;
  if (this->value().size() > 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::BytesSize(
        this->value());
  }

  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void Any::MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:google.protobuf.Any)
  GOOGLE_DCHECK_NE(&from, this);
  const Any* source =
      ::PROTOBUF_NAMESPACE_ID::DynamicCastToGenerated<Any>(
          &from);
  if (source == nullptr) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:google.protobuf.Any)
    ::PROTOBUF_NAMESPACE_ID::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:google.protobuf.Any)
    MergeFrom(*source);
  }
}

void Any::MergeFrom(const Any& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:google.protobuf.Any)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (from.type_url().size() > 0) {

    type_url_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.type_url_);
  }
  if (from.value().size() > 0) {

    value_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.value_);
  }
}

void Any::CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:google.protobuf.Any)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void Any::CopyFrom(const Any& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:google.protobuf.Any)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool Any::IsInitialized() const {
  return true;
}

void Any::InternalSwap(Any* other) {
  using std::swap;
  _internal_metadata_.Swap(&other->_internal_metadata_);
  type_url_.Swap(&other->type_url_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
    GetArenaNoVirtual());
  value_.Swap(&other->value_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
    GetArenaNoVirtual());
}

::PROTOBUF_NAMESPACE_ID::Metadata Any::GetMetadata() const {
  return GetMetadataStatic();
}


// @@protoc_insertion_point(namespace_scope)
PROTOBUF_NAMESPACE_CLOSE
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE PROTOBUF_NAMESPACE_ID::Any* Arena::CreateMaybeMessage< PROTOBUF_NAMESPACE_ID::Any >(Arena* arena) {
  return Arena::CreateInternal< PROTOBUF_NAMESPACE_ID::Any >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>
