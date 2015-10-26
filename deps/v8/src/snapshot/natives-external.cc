// Copyright 2014 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/snapshot/natives.h"

#include "src/base/logging.h"
#include "src/list.h"
#include "src/list-inl.h"
#include "src/snapshot/snapshot-source-sink.h"
#include "src/vector.h"

#ifndef V8_USE_EXTERNAL_STARTUP_DATA
#error natives-external.cc is used only for the external snapshot build.
#endif  // V8_USE_EXTERNAL_STARTUP_DATA


namespace v8 {
namespace internal {


/**
 * NativesStore stores the 'native' (builtin) JS libraries.
 *
 * NativesStore needs to be initialized before using V8, usually by the
 * embedder calling v8::SetNativesDataBlob, which calls SetNativesFromFile
 * below.
 */
class NativesStore {
 public:
  ~NativesStore() {
    for (int i = 0; i < native_names_.length(); i++) {
      native_names_[i].Dispose();
    }
  }

  int GetBuiltinsCount() { return native_ids_.length(); }
  int GetDebuggerCount() { return debugger_count_; }

  Vector<const char> GetScriptSource(int index) {
    return native_source_[index];
  }

  Vector<const char> GetScriptName(int index) { return native_names_[index]; }

  int GetIndex(const char* id) {
    for (int i = 0; i < native_ids_.length(); ++i) {
      int native_id_length = native_ids_[i].length();
      if ((static_cast<int>(strlen(id)) == native_id_length) &&
          (strncmp(id, native_ids_[i].start(), native_id_length) == 0)) {
        return i;
      }
    }
    DCHECK(false);
    return -1;
  }

  Vector<const char> GetScriptsSource() {
    DCHECK(false);  // Not implemented.
    return Vector<const char>();
  }

  static NativesStore* MakeFromScriptsSource(SnapshotByteSource* source) {
    NativesStore* store = new NativesStore;

    // We expect the libraries in the following format:
    //   int: # of debugger sources.
    //   2N blobs: N pairs of source name + actual source.
    //   then, repeat for non-debugger sources.
    int debugger_count = source->GetInt();
    for (int i = 0; i < debugger_count; ++i)
      store->ReadNameAndContentPair(source);
    int library_count = source->GetInt();
    for (int i = 0; i < library_count; ++i)
      store->ReadNameAndContentPair(source);

    store->debugger_count_ = debugger_count;
    return store;
  }

 private:
  NativesStore() : debugger_count_(0) {}

  Vector<const char> NameFromId(const byte* id, int id_length) {
    const char native[] = "native ";
    const char extension[] = ".js";
    Vector<char> name(Vector<char>::New(id_length + sizeof(native) - 1 +
                                        sizeof(extension) - 1));
    memcpy(name.start(), native, sizeof(native) - 1);
    memcpy(name.start() + sizeof(native) - 1, id, id_length);
    memcpy(name.start() + sizeof(native) - 1 + id_length, extension,
           sizeof(extension) - 1);
    return Vector<const char>::cast(name);
  }

  bool ReadNameAndContentPair(SnapshotByteSource* bytes) {
    const byte* id;
    int id_length;
    const byte* source;
    int source_length;
    bool success = bytes->GetBlob(&id, &id_length) &&
                   bytes->GetBlob(&source, &source_length);
    if (success) {
      Vector<const char> id_vector(reinterpret_cast<const char*>(id),
                                   id_length);
      Vector<const char> source_vector(
          reinterpret_cast<const char*>(source), source_length);
      native_ids_.Add(id_vector);
      native_source_.Add(source_vector);
      native_names_.Add(NameFromId(id, id_length));
    }
    return success;
  }

  List<Vector<const char> > native_ids_;
  List<Vector<const char> > native_names_;
  List<Vector<const char> > native_source_;
  int debugger_count_;

  DISALLOW_COPY_AND_ASSIGN(NativesStore);
};


template<NativeType type>
class NativesHolder {
 public:
  static NativesStore* get() {
    DCHECK(holder_);
    return holder_;
  }
  static void set(NativesStore* store) {
    DCHECK(store);
    holder_ = store;
  }
  static bool empty() { return holder_ == NULL; }
  static void Dispose() {
    delete holder_;
    holder_ = NULL;
  }

 private:
  static NativesStore* holder_;
};

template<NativeType type>
NativesStore* NativesHolder<type>::holder_ = NULL;


// The natives blob. Memory is owned by caller.
static StartupData* natives_blob_ = NULL;


/**
 * Read the Natives blob, as previously set by SetNativesFromFile.
 */
void ReadNatives() {
  if (natives_blob_ && NativesHolder<CORE>::empty()) {
    SnapshotByteSource bytes(natives_blob_->data, natives_blob_->raw_size);
    NativesHolder<CORE>::set(NativesStore::MakeFromScriptsSource(&bytes));
    NativesHolder<CODE_STUB>::set(NativesStore::MakeFromScriptsSource(&bytes));
    NativesHolder<EXPERIMENTAL>::set(
        NativesStore::MakeFromScriptsSource(&bytes));
    NativesHolder<EXTRAS>::set(NativesStore::MakeFromScriptsSource(&bytes));
    NativesHolder<EXPERIMENTAL_EXTRAS>::set(
        NativesStore::MakeFromScriptsSource(&bytes));
    DCHECK(!bytes.HasMore());
  }
}


/**
 * Set the Natives (library sources) blob, as generated by js2c + the build
 * system.
 */
void SetNativesFromFile(StartupData* natives_blob) {
  DCHECK(!natives_blob_);
  DCHECK(natives_blob);
  DCHECK(natives_blob->data);
  DCHECK(natives_blob->raw_size > 0);

  natives_blob_ = natives_blob;
  ReadNatives();
}


/**
 * Release memory allocated by SetNativesFromFile.
 */
void DisposeNatives() {
  NativesHolder<CORE>::Dispose();
  NativesHolder<CODE_STUB>::Dispose();
  NativesHolder<EXPERIMENTAL>::Dispose();
  NativesHolder<EXTRAS>::Dispose();
  NativesHolder<EXPERIMENTAL_EXTRAS>::Dispose();
}


// Implement NativesCollection<T> bsaed on NativesHolder + NativesStore.
//
// (The callers expect a purely static interface, since this is how the
//  natives are usually compiled in. Since we implement them based on
//  runtime content, we have to implement this indirection to offer
//  a static interface.)
template<NativeType type>
int NativesCollection<type>::GetBuiltinsCount() {
  return NativesHolder<type>::get()->GetBuiltinsCount();
}

template<NativeType type>
int NativesCollection<type>::GetDebuggerCount() {
  return NativesHolder<type>::get()->GetDebuggerCount();
}

template<NativeType type>
int NativesCollection<type>::GetIndex(const char* name) {
  return NativesHolder<type>::get()->GetIndex(name);
}

template <NativeType type>
Vector<const char> NativesCollection<type>::GetScriptSource(int index) {
  return NativesHolder<type>::get()->GetScriptSource(index);
}

template<NativeType type>
Vector<const char> NativesCollection<type>::GetScriptName(int index) {
  return NativesHolder<type>::get()->GetScriptName(index);
}

template <NativeType type>
Vector<const char> NativesCollection<type>::GetScriptsSource() {
  return NativesHolder<type>::get()->GetScriptsSource();
}


// Explicit template instantiations.
#define INSTANTIATE_TEMPLATES(T)                                            \
  template int NativesCollection<T>::GetBuiltinsCount();                    \
  template int NativesCollection<T>::GetDebuggerCount();                    \
  template int NativesCollection<T>::GetIndex(const char* name);            \
  template Vector<const char> NativesCollection<T>::GetScriptSource(int i); \
  template Vector<const char> NativesCollection<T>::GetScriptName(int i);   \
  template Vector<const char> NativesCollection<T>::GetScriptsSource();
INSTANTIATE_TEMPLATES(CORE)
INSTANTIATE_TEMPLATES(CODE_STUB)
INSTANTIATE_TEMPLATES(EXPERIMENTAL)
INSTANTIATE_TEMPLATES(EXTRAS)
INSTANTIATE_TEMPLATES(EXPERIMENTAL_EXTRAS)
#undef INSTANTIATE_TEMPLATES

}  // namespace internal
}  // namespace v8
