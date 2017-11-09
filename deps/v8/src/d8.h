// Copyright 2012 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_D8_H_
#define V8_D8_H_

#include <iterator>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "src/allocation.h"
#include "src/base/hashmap.h"
#include "src/base/platform/time.h"
#include "src/utils.h"

#include "src/base/once.h"


namespace v8 {


// A single counter in a counter collection.
class Counter {
 public:
  static const int kMaxNameSize = 64;
  int32_t* Bind(const char* name, bool histogram);
  int32_t* ptr() { return &count_; }
  int32_t count() { return count_; }
  int32_t sample_total() { return sample_total_; }
  bool is_histogram() { return is_histogram_; }
  void AddSample(int32_t sample);
 private:
  int32_t count_;
  int32_t sample_total_;
  bool is_histogram_;
  uint8_t name_[kMaxNameSize];
};


// A set of counters and associated information.  An instance of this
// class is stored directly in the memory-mapped counters file if
// the --map-counters options is used
class CounterCollection {
 public:
  CounterCollection();
  Counter* GetNextCounter();
 private:
  static const unsigned kMaxCounters = 512;
  uint32_t magic_number_;
  uint32_t max_counters_;
  uint32_t max_name_size_;
  uint32_t counters_in_use_;
  Counter counters_[kMaxCounters];
};


class CounterMap {
 public:
  CounterMap(): hash_map_(Match) { }
  Counter* Lookup(const char* name) {
    base::HashMap::Entry* answer =
        hash_map_.Lookup(const_cast<char*>(name), Hash(name));
    if (!answer) return NULL;
    return reinterpret_cast<Counter*>(answer->value);
  }
  void Set(const char* name, Counter* value) {
    base::HashMap::Entry* answer =
        hash_map_.LookupOrInsert(const_cast<char*>(name), Hash(name));
    DCHECK(answer != NULL);
    answer->value = value;
  }
  class Iterator {
   public:
    explicit Iterator(CounterMap* map)
        : map_(&map->hash_map_), entry_(map_->Start()) { }
    void Next() { entry_ = map_->Next(entry_); }
    bool More() { return entry_ != NULL; }
    const char* CurrentKey() { return static_cast<const char*>(entry_->key); }
    Counter* CurrentValue() { return static_cast<Counter*>(entry_->value); }
   private:
    base::CustomMatcherHashMap* map_;
    base::CustomMatcherHashMap::Entry* entry_;
  };

 private:
  static int Hash(const char* name);
  static bool Match(void* key1, void* key2);
  base::CustomMatcherHashMap hash_map_;
};


class SourceGroup {
 public:
  SourceGroup() :
      next_semaphore_(0),
      done_semaphore_(0),
      thread_(NULL),
      argv_(NULL),
      begin_offset_(0),
      end_offset_(0) {}

  ~SourceGroup();

  void Begin(char** argv, int offset) {
    argv_ = const_cast<const char**>(argv);
    begin_offset_ = offset;
  }

  void End(int offset) { end_offset_ = offset; }

  void Execute(Isolate* isolate);

  void StartExecuteInThread();
  void WaitForThread();
  void JoinThread();

 private:
  class IsolateThread : public base::Thread {
   public:
    explicit IsolateThread(SourceGroup* group)
        : base::Thread(GetThreadOptions()), group_(group) {}

    virtual void Run() {
      group_->ExecuteInThread();
    }

   private:
    SourceGroup* group_;
  };

  static base::Thread::Options GetThreadOptions();
  void ExecuteInThread();

  base::Semaphore next_semaphore_;
  base::Semaphore done_semaphore_;
  base::Thread* thread_;

  void ExitShell(int exit_code);
  Local<String> ReadFile(Isolate* isolate, const char* name);

  const char** argv_;
  int begin_offset_;
  int end_offset_;
};

// The backing store of an ArrayBuffer or SharedArrayBuffer, after
// Externalize() has been called on it.
class ExternalizedContents {
 public:
  explicit ExternalizedContents(const ArrayBuffer::Contents& contents)
      : data_(contents.Data()), size_(contents.ByteLength()) {}
  explicit ExternalizedContents(const SharedArrayBuffer::Contents& contents)
      : data_(contents.Data()), size_(contents.ByteLength()) {}
  ExternalizedContents(ExternalizedContents&& other)
      : data_(other.data_), size_(other.size_) {
    other.data_ = nullptr;
    other.size_ = 0;
  }
  ExternalizedContents& operator=(ExternalizedContents&& other) {
    if (this != &other) {
      data_ = other.data_;
      size_ = other.size_;
      other.data_ = nullptr;
      other.size_ = 0;
    }
    return *this;
  }
  ~ExternalizedContents();

 private:
  void* data_;
  size_t size_;

  DISALLOW_COPY_AND_ASSIGN(ExternalizedContents);
};

class SerializationData {
 public:
  SerializationData() : size_(0) {}

  uint8_t* data() { return data_.get(); }
  size_t size() { return size_; }
  const std::vector<ArrayBuffer::Contents>& array_buffer_contents() {
    return array_buffer_contents_;
  }
  const std::vector<SharedArrayBuffer::Contents>&
  shared_array_buffer_contents() {
    return shared_array_buffer_contents_;
  }

  void AppendExternalizedContentsTo(std::vector<ExternalizedContents>* to) {
    to->insert(to->end(),
               std::make_move_iterator(externalized_contents_.begin()),
               std::make_move_iterator(externalized_contents_.end()));
    externalized_contents_.clear();
  }

 private:
  struct DataDeleter {
    void operator()(uint8_t* p) const { free(p); }
  };

  std::unique_ptr<uint8_t, DataDeleter> data_;
  size_t size_;
  std::vector<ArrayBuffer::Contents> array_buffer_contents_;
  std::vector<SharedArrayBuffer::Contents> shared_array_buffer_contents_;
  std::vector<ExternalizedContents> externalized_contents_;

 private:
  friend class Serializer;

  DISALLOW_COPY_AND_ASSIGN(SerializationData);
};


class SerializationDataQueue {
 public:
  void Enqueue(std::unique_ptr<SerializationData> data);
  bool Dequeue(std::unique_ptr<SerializationData>* data);
  bool IsEmpty();
  void Clear();

 private:
  base::Mutex mutex_;
  std::vector<std::unique_ptr<SerializationData>> data_;
};


class Worker {
 public:
  Worker();
  ~Worker();

  // Run the given script on this Worker. This function should only be called
  // once, and should only be called by the thread that created the Worker.
  void StartExecuteInThread(const char* script);
  // Post a message to the worker's incoming message queue. The worker will
  // take ownership of the SerializationData.
  // This function should only be called by the thread that created the Worker.
  void PostMessage(std::unique_ptr<SerializationData> data);
  // Synchronously retrieve messages from the worker's outgoing message queue.
  // If there is no message in the queue, block until a message is available.
  // If there are no messages in the queue and the worker is no longer running,
  // return nullptr.
  // This function should only be called by the thread that created the Worker.
  std::unique_ptr<SerializationData> GetMessage();
  // Terminate the worker's event loop. Messages from the worker that have been
  // queued can still be read via GetMessage().
  // This function can be called by any thread.
  void Terminate();
  // Terminate and join the thread.
  // This function can be called by any thread.
  void WaitForThread();

 private:
  class WorkerThread : public base::Thread {
   public:
    explicit WorkerThread(Worker* worker)
        : base::Thread(base::Thread::Options("WorkerThread")),
          worker_(worker) {}

    virtual void Run() { worker_->ExecuteInThread(); }

   private:
    Worker* worker_;
  };

  void ExecuteInThread();
  static void PostMessageOut(const v8::FunctionCallbackInfo<v8::Value>& args);

  base::Semaphore in_semaphore_;
  base::Semaphore out_semaphore_;
  SerializationDataQueue in_queue_;
  SerializationDataQueue out_queue_;
  base::Thread* thread_;
  char* script_;
  base::Atomic32 running_;
};

class ShellOptions {
 public:
  ShellOptions()
      : script_executed(false),
        send_idle_notification(false),
        invoke_weak_callbacks(false),
        omit_quit(false),
        stress_opt(false),
        stress_deopt(false),
        stress_runs(1),
        interactive_shell(false),
        test_shell(false),
        expected_to_throw(false),
        mock_arraybuffer_allocator(false),
        enable_inspector(false),
        num_isolates(1),
        compile_options(v8::ScriptCompiler::kNoCompileOptions),
        isolate_sources(NULL),
        icu_data_file(NULL),
        natives_blob(NULL),
        snapshot_blob(NULL),
        trace_enabled(false),
        trace_config(NULL),
        lcov_file(NULL),
        disable_in_process_stack_traces(false),
        read_from_tcp_port(-1) {}

  ~ShellOptions() {
    delete[] isolate_sources;
  }

  bool use_interactive_shell() {
    return (interactive_shell || !script_executed) && !test_shell;
  }

  bool script_executed;
  bool send_idle_notification;
  bool invoke_weak_callbacks;
  bool omit_quit;
  bool stress_opt;
  bool stress_deopt;
  int stress_runs;
  bool interactive_shell;
  bool test_shell;
  bool expected_to_throw;
  bool mock_arraybuffer_allocator;
  bool enable_inspector;
  int num_isolates;
  v8::ScriptCompiler::CompileOptions compile_options;
  SourceGroup* isolate_sources;
  const char* icu_data_file;
  const char* natives_blob;
  const char* snapshot_blob;
  bool trace_enabled;
  const char* trace_config;
  const char* lcov_file;
  bool disable_in_process_stack_traces;
  int read_from_tcp_port;
  bool enable_os_system = false;
};

class Shell : public i::AllStatic {
 public:
  static MaybeLocal<Script> CompileString(
      Isolate* isolate, Local<String> source, Local<Value> name,
      v8::ScriptCompiler::CompileOptions compile_options);
  static bool ExecuteString(Isolate* isolate, Local<String> source,
                            Local<Value> name, bool print_result,
                            bool report_exceptions);
  static bool ExecuteModule(Isolate* isolate, const char* file_name);
  static void ReportException(Isolate* isolate, TryCatch* try_catch);
  static Local<String> ReadFile(Isolate* isolate, const char* name);
  static Local<Context> CreateEvaluationContext(Isolate* isolate);
  static int RunMain(Isolate* isolate, int argc, char* argv[], bool last_run);
  static int Main(int argc, char* argv[]);
  static void Exit(int exit_code);
  static void OnExit(Isolate* isolate);
  static void CollectGarbage(Isolate* isolate);
  static void EmptyMessageQueues(Isolate* isolate);
  static void EnsureEventLoopInitialized(Isolate* isolate);
  static void CompleteMessageLoop(Isolate* isolate);

  static std::unique_ptr<SerializationData> SerializeValue(
      Isolate* isolate, Local<Value> value, Local<Value> transfer);
  static MaybeLocal<Value> DeserializeValue(
      Isolate* isolate, std::unique_ptr<SerializationData> data);
  static void CleanupWorkers();
  static int* LookupCounter(const char* name);
  static void* CreateHistogram(const char* name,
                               int min,
                               int max,
                               size_t buckets);
  static void AddHistogramSample(void* histogram, int sample);
  static void MapCounters(v8::Isolate* isolate, const char* name);

  static void PerformanceNow(const v8::FunctionCallbackInfo<v8::Value>& args);

  static void RealmCurrent(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void RealmOwner(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void RealmGlobal(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void RealmCreate(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void RealmNavigate(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void RealmCreateAllowCrossRealmAccess(
      const v8::FunctionCallbackInfo<v8::Value>& args);
  static void RealmDispose(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void RealmSwitch(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void RealmEval(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void RealmSharedGet(Local<String> property,
                             const  PropertyCallbackInfo<Value>& info);
  static void RealmSharedSet(Local<String> property,
                             Local<Value> value,
                             const  PropertyCallbackInfo<void>& info);

  static void Print(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void PrintErr(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void Write(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void WaitUntilDone(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void NotifyDone(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void QuitOnce(v8::FunctionCallbackInfo<v8::Value>* args);
  static void Quit(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void Version(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void Read(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void ReadBuffer(const v8::FunctionCallbackInfo<v8::Value>& args);
  static Local<String> ReadFromStdin(Isolate* isolate);
  static void ReadLine(const v8::FunctionCallbackInfo<v8::Value>& args) {
    args.GetReturnValue().Set(ReadFromStdin(args.GetIsolate()));
  }
  static void Load(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void WorkerNew(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void WorkerPostMessage(
      const v8::FunctionCallbackInfo<v8::Value>& args);
  static void WorkerGetMessage(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void WorkerTerminate(const v8::FunctionCallbackInfo<v8::Value>& args);
  // The OS object on the global object contains methods for performing
  // operating system calls:
  //
  // os.system("program_name", ["arg1", "arg2", ...], timeout1, timeout2) will
  // run the command, passing the arguments to the program.  The standard output
  // of the program will be picked up and returned as a multiline string.  If
  // timeout1 is present then it should be a number.  -1 indicates no timeout
  // and a positive number is used as a timeout in milliseconds that limits the
  // time spent waiting between receiving output characters from the program.
  // timeout2, if present, should be a number indicating the limit in
  // milliseconds on the total running time of the program.  Exceptions are
  // thrown on timeouts or other errors or if the exit status of the program
  // indicates an error.
  //
  // os.chdir(dir) changes directory to the given directory.  Throws an
  // exception/ on error.
  //
  // os.setenv(variable, value) sets an environment variable.  Repeated calls to
  // this method leak memory due to the API of setenv in the standard C library.
  //
  // os.umask(alue) calls the umask system call and returns the old umask.
  //
  // os.mkdirp(name, mask) creates a directory.  The mask (if present) is anded
  // with the current umask.  Intermediate directories are created if necessary.
  // An exception is not thrown if the directory already exists.  Analogous to
  // the "mkdir -p" command.
  static void System(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void ChangeDirectory(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void SetEnvironment(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void UnsetEnvironment(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void SetUMask(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void MakeDirectory(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void RemoveDirectory(const v8::FunctionCallbackInfo<v8::Value>& args);
  static MaybeLocal<Promise> HostImportModuleDynamically(
      Local<Context> context, Local<ScriptOrModule> referrer,
      Local<String> specifier);

  // Data is of type DynamicImportData*. We use void* here to be able
  // to conform with MicrotaskCallback interface and enqueue this
  // function in the microtask queue.
  static void DoHostImportModuleDynamically(void* data);
  static void AddOSMethods(v8::Isolate* isolate,
                           Local<ObjectTemplate> os_template);

  static const char* kPrompt;
  static ShellOptions options;
  static ArrayBuffer::Allocator* array_buffer_allocator;

  static void SetWaitUntilDone(Isolate* isolate, bool value);
  static bool IsWaitUntilDone(Isolate* isolate);

  static char* ReadCharsFromTcpPort(const char* name, int* size_out);

 private:
  static Global<Context> evaluation_context_;
  static base::OnceType quit_once_;
  static Global<Function> stringify_function_;
  static CounterMap* counter_map_;
  // We statically allocate a set of local counters to be used if we
  // don't want to store the stats in a memory-mapped file
  static CounterCollection local_counters_;
  static CounterCollection* counters_;
  static base::OS::MemoryMappedFile* counters_file_;
  static base::LazyMutex context_mutex_;
  static const base::TimeTicks kInitialTicks;

  static base::LazyMutex workers_mutex_;
  static bool allow_new_workers_;
  static std::vector<Worker*> workers_;
  static std::vector<ExternalizedContents> externalized_contents_;

  static void WriteIgnitionDispatchCountersFile(v8::Isolate* isolate);
  // Append LCOV coverage data to file.
  static void WriteLcovData(v8::Isolate* isolate, const char* file);
  static Counter* GetCounter(const char* name, bool is_histogram);
  static Local<String> Stringify(Isolate* isolate, Local<Value> value);
  static void Initialize(Isolate* isolate);
  static void RunShell(Isolate* isolate);
  static bool SetOptions(int argc, char* argv[]);
  static Local<ObjectTemplate> CreateGlobalTemplate(Isolate* isolate);
  static MaybeLocal<Context> CreateRealm(
      const v8::FunctionCallbackInfo<v8::Value>& args, int index,
      v8::MaybeLocal<Value> global_object);
  static void DisposeRealm(const v8::FunctionCallbackInfo<v8::Value>& args,
                           int index);
  static MaybeLocal<Module> FetchModuleTree(v8::Local<v8::Context> context,
                                            const std::string& file_name);
  // We may have multiple isolates running concurrently, so the access to
  // the isolate_status_ needs to be concurrency-safe.
  static base::LazyMutex isolate_status_lock_;
  static std::map<Isolate*, bool> isolate_status_;
};


}  // namespace v8


#endif  // V8_D8_H_
