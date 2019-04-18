#include "snapshot_builder.h"
#include <iostream>
#include <sstream>
#include "env-inl.h"
#include "node_internals.h"
#include "node_main_instance.h"
#include "node_v8_platform-inl.h"

namespace node {

using v8::Context;
using v8::HandleScope;
using v8::Isolate;
using v8::Local;
using v8::Locker;
using v8::SnapshotCreator;
using v8::StartupData;

std::string FormatBlob(v8::StartupData* blob,
                       const std::vector<size_t>& isolate_data_indexes) {
  std::stringstream ss;
  size_t isolate_data_indexes_size = isolate_data_indexes.size();

  ss << R"(#include <cinttypes>
#include <array>
#include "node_main_instance.h"
#include "v8.h"

// This file is generated by tools/snapshot. Do not edit.

namespace node {

static const uint8_t blob_data[] = {
)";

  for (int i = 0; i < blob->raw_size; i++) {
    uint8_t ch = blob->data[i];
    ss << std::to_string(ch) << ((i == blob->raw_size - 1) ? '\n' : ',');
  }

  ss << R"(};

static const int blob_size = )"
     << blob->raw_size << R"(;
static v8::StartupData blob = {
  reinterpret_cast<const char*>(blob_data),
  blob_size
};
)";

  ss << R"(v8::StartupData*
NodeMainInstance::GetEmbeddedSnapshotBlob() {
  return &blob;
}

static const size_t isolate_data_indexes_raw[] = {
)";
  for (size_t i = 0; i < isolate_data_indexes_size; i++) {
    ss << std::to_string(isolate_data_indexes[i])
       << ((i == isolate_data_indexes_size - 1) ? '\n' : ',');
  }
  ss << "};\n\n";

  ss << "static const size_t isolate_data_indexes_size = "
     << isolate_data_indexes_size << R"(;

NodeMainInstance::IndexArray isolate_data_indexes {
    isolate_data_indexes_raw,
    isolate_data_indexes_size
};

const NodeMainInstance::IndexArray*
NodeMainInstance::GetIsolateDataIndexes() {
  return &isolate_data_indexes;
}
}  // namespace node
)";

  return ss.str();
}

std::string SnapshotBuilder::Generate(
    const std::vector<std::string> args,
    const std::vector<std::string> exec_args) {
  // TODO(joyeecheung): collect external references and set it in
  // params.external_references.
  std::vector<intptr_t> external_references = {
      reinterpret_cast<intptr_t>(nullptr)};
  Isolate* isolate = Isolate::Allocate();
  per_process::v8_platform.Platform()->RegisterIsolate(isolate,
                                                       uv_default_loop());
  NodeMainInstance* main_instance = nullptr;
  std::string result;

  {
    std::vector<size_t> isolate_data_indexes;
    SnapshotCreator creator(isolate, external_references.data());
    {
      main_instance =
          NodeMainInstance::Create(isolate,
                                   uv_default_loop(),
                                   per_process::v8_platform.Platform(),
                                   args,
                                   exec_args);
      HandleScope scope(isolate);
      creator.SetDefaultContext(Context::New(isolate));
      isolate_data_indexes = main_instance->isolate_data()->Serialize(&creator);
    }

    // Must be out of HandleScope
    StartupData blob =
        creator.CreateBlob(SnapshotCreator::FunctionCodeHandling::kClear);
    // Must be done while the snapshot creator isolate is entered i.e. the
    // creator is still alive.
    main_instance->Dispose();
    result = FormatBlob(&blob, isolate_data_indexes);
  }

  per_process::v8_platform.Platform()->UnregisterIsolate(isolate);
  return result;
}
}  // namespace node
