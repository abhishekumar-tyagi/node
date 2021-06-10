#include "env-inl.h"
#include "json_utils.h"
#include "node_report.h"
#include "debug_utils-inl.h"
#include "diagnosticfilename-inl.h"
#include "node_internals.h"
#include "node_metadata.h"
#include "node_mutex.h"
#include "node_worker.h"
#include "util.h"

#ifdef _WIN32
#include <Windows.h>
#else  // !_WIN32
#include <cxxabi.h>
#include <sys/resource.h>
#include <dlfcn.h>
#endif

#include <iostream>
#include <cstring>
#include <ctime>
#include <cwctype>
#include <fstream>

constexpr int NODE_REPORT_VERSION = 2;
constexpr int NANOS_PER_SEC = 1000 * 1000 * 1000;
constexpr double SEC_PER_MICROS = 1e-6;

namespace report {
using node::arraysize;
using node::ConditionVariable;
using node::DiagnosticFilename;
using node::Environment;
using node::JSONWriter;
using node::Mutex;
using node::NativeSymbolDebuggingContext;
using node::TIME_TYPE;
using node::worker::Worker;
using v8::Array;
using v8::Context;
using v8::HeapSpaceStatistics;
using v8::HeapStatistics;
using v8::Isolate;
using v8::Local;
using v8::Object;
using v8::String;
using v8::TryCatch;
using v8::V8;
using v8::Value;

namespace per_process = node::per_process;

// Internal/static function declarations
static void WriteNodeReport(Isolate* isolate,
                            Environment* env,
                            const char* message,
                            const char* trigger,
                            const std::string& filename,
                            std::ostream& out,
                            Local<Object> error,
                            bool compact);
static void PrintVersionInformation(JSONWriter* writer);
static void PrintJavaScriptErrorStack(JSONWriter* writer,
                                      Isolate* isolate,
                                      Local<Object> error,
                                      const char* trigger);
static void PrintJavaScriptErrorProperties(JSONWriter* writer,
                                           Isolate* isolate,
                                           Local<Object> error);
static void PrintNativeStack(JSONWriter* writer);
static void PrintResourceUsage(JSONWriter* writer);
static void PrintGCStatistics(JSONWriter* writer, Isolate* isolate);
static void PrintSystemInformation(JSONWriter* writer);
static void PrintLoadedLibraries(JSONWriter* writer);
static void PrintComponentVersions(JSONWriter* writer);
static void PrintRelease(JSONWriter* writer);
static void PrintCpuInfo(JSONWriter* writer);
static void PrintNetworkInterfaceInfo(JSONWriter* writer);

// External function to trigger a report, writing to file.
std::string TriggerNodeReport(Isolate* isolate,
                              Environment* env,
                              const char* message,
                              const char* trigger,
                              const std::string& name,
                              Local<Object> error) {
  std::string filename;

  // Determine the required report filename. In order of priority:
  //   1) supplied on API 2) configured on startup 3) default generated
  if (!name.empty()) {
    // Filename was specified as API parameter.
    filename = name;
  } else {
    std::string report_filename;
    {
      Mutex::ScopedLock lock(per_process::cli_options_mutex);
      report_filename = per_process::cli_options->report_filename;
    }
    if (report_filename.length() > 0) {
      // File name was supplied via start-up option.
      filename = report_filename;
    } else {
      filename = *DiagnosticFilename(env != nullptr ? env->thread_id() : 0,
          "report", "json");
    }
  }

  // Open the report file stream for writing. Supports stdout/err,
  // user-specified or (default) generated name
  std::ofstream outfile;
  std::ostream* outstream;
  if (filename == "stdout") {
    outstream = &std::cout;
  } else if (filename == "stderr") {
    outstream = &std::cerr;
  } else {
    std::string report_directory;
    {
      Mutex::ScopedLock lock(per_process::cli_options_mutex);
      report_directory = per_process::cli_options->report_directory;
    }
    // Regular file. Append filename to directory path if one was specified
    if (report_directory.length() > 0) {
      std::string pathname = report_directory;
      pathname += node::kPathSeparator;
      pathname += filename;
      outfile.open(pathname, std::ios::out | std::ios::binary);
    } else {
      outfile.open(filename, std::ios::out | std::ios::binary);
    }
    // Check for errors on the file open
    if (!outfile.is_open()) {
      std::cerr << "\nFailed to open Node.js report file: " << filename;

      if (report_directory.length() > 0)
        std::cerr << " directory: " << report_directory;

      std::cerr << " (errno: " << errno << ")" << std::endl;
      return "";
    }
    outstream = &outfile;
    std::cerr << "\nWriting Node.js report to file: " << filename;
  }

  bool compact;
  {
    Mutex::ScopedLock lock(per_process::cli_options_mutex);
    compact = per_process::cli_options->report_compact;
  }
  WriteNodeReport(isolate, env, message, trigger, filename, *outstream,
                  error, compact);

  // Do not close stdout/stderr, only close files we opened.
  if (outfile.is_open()) {
    outfile.close();
  }

  // Do not mix JSON and free-form text on stderr.
  if (filename != "stderr") {
    std::cerr << "\nNode.js report completed" << std::endl;
  }
  return filename;
}

// External function to trigger a report, writing to a supplied stream.
void GetNodeReport(Isolate* isolate,
                   Environment* env,
                   const char* message,
                   const char* trigger,
                   Local<Object> error,
                   std::ostream& out) {
  WriteNodeReport(isolate, env, message, trigger, "", out, error, false);
}

// Internal function to coordinate and write the various
// sections of the report to the supplied stream
static void WriteNodeReport(Isolate* isolate,
                            Environment* env,
                            const char* message,
                            const char* trigger,
                            const std::string& filename,
                            std::ostream& out,
                            Local<Object> error,
                            bool compact) {
  // Obtain the current time and the pid.
  TIME_TYPE tm_struct;
  DiagnosticFilename::LocalTime(&tm_struct);
  uv_pid_t pid = uv_os_getpid();

  // Save formatting for output stream.
  std::ios old_state(nullptr);
  old_state.copyfmt(out);

  // File stream opened OK, now start printing the report content:
  // the title and header information (event, filename, timestamp and pid)

  JSONWriter writer(out, compact);
  writer.json_start();
  writer.json_objectstart("header");
  writer.json_keyvalue("reportVersion", NODE_REPORT_VERSION);
  writer.json_keyvalue("event", message);
  writer.json_keyvalue("trigger", trigger);
  if (!filename.empty())
    writer.json_keyvalue("filename", filename);
  else
    writer.json_keyvalue("filename", JSONWriter::Null{});

  // Report dump event and module load date/time stamps
  char timebuf[64];
#ifdef _WIN32
  snprintf(timebuf,
           sizeof(timebuf),
           "%4d-%02d-%02dT%02d:%02d:%02dZ",
           tm_struct.wYear,
           tm_struct.wMonth,
           tm_struct.wDay,
           tm_struct.wHour,
           tm_struct.wMinute,
           tm_struct.wSecond);
  writer.json_keyvalue("dumpEventTime", timebuf);
#else  // UNIX, OSX
  snprintf(timebuf,
           sizeof(timebuf),
           "%4d-%02d-%02dT%02d:%02d:%02dZ",
           tm_struct.tm_year + 1900,
           tm_struct.tm_mon + 1,
           tm_struct.tm_mday,
           tm_struct.tm_hour,
           tm_struct.tm_min,
           tm_struct.tm_sec);
  writer.json_keyvalue("dumpEventTime", timebuf);
#endif

  uv_timeval64_t ts;
  if (uv_gettimeofday(&ts) == 0) {
    writer.json_keyvalue("dumpEventTimeStamp",
                         std::to_string(ts.tv_sec * 1000 + ts.tv_usec / 1000));
  }

  // Report native process ID
  writer.json_keyvalue("processId", pid);
  if (env != nullptr)
    writer.json_keyvalue("threadId", env->thread_id());
  else
    writer.json_keyvalue("threadId", JSONWriter::Null{});

  {
    // Report the process cwd.
    char buf[PATH_MAX_BYTES];
    size_t cwd_size = sizeof(buf);
    if (uv_cwd(buf, &cwd_size) == 0)
      writer.json_keyvalue("cwd", buf);
  }

  // Report out the command line.
  if (!node::per_process::cli_options->cmdline.empty()) {
    writer.json_arraystart("commandLine");
    for (const std::string& arg : node::per_process::cli_options->cmdline) {
      writer.json_element(arg);
    }
    writer.json_arrayend();
  }

  // Report Node.js and OS version information
  PrintVersionInformation(&writer);
  writer.json_objectend();

  if (isolate != nullptr) {
    writer.json_objectstart("javascriptStack");
    // Report summary JavaScript error stack backtrace
    PrintJavaScriptErrorStack(&writer, isolate, error, trigger);

    // Report summary JavaScript error properties backtrace
    PrintJavaScriptErrorProperties(&writer, isolate, error);
    writer.json_objectend();  // the end of 'javascriptStack'

    // Report V8 Heap and Garbage Collector information
    PrintGCStatistics(&writer, isolate);
  }

  // Report native stack backtrace
  PrintNativeStack(&writer);

  // Report OS and current thread resource usage
  PrintResourceUsage(&writer);

  writer.json_arraystart("libuv");
  if (env != nullptr) {
    uv_walk(env->event_loop(), WalkHandle, static_cast<void*>(&writer));

    writer.json_start();
    writer.json_keyvalue("type", "loop");
    writer.json_keyvalue("is_active",
        static_cast<bool>(uv_loop_alive(env->event_loop())));
    writer.json_keyvalue("address",
        ValueToHexString(reinterpret_cast<int64_t>(env->event_loop())));

    // Report Event loop idle time
    uint64_t idle_time = uv_metrics_idle_time(env->event_loop());
    writer.json_keyvalue("loopIdleTimeSeconds", 1.0 * idle_time / 1e9);
    writer.json_end();
  }

  writer.json_arrayend();

  writer.json_arraystart("workers");
  if (env != nullptr) {
    Mutex workers_mutex;
    ConditionVariable notify;
    std::vector<std::string> worker_infos;
    size_t expected_results = 0;

    env->ForEachWorker([&](Worker* w) {
      expected_results += w->RequestInterrupt([&](Environment* env) {
        std::ostringstream os;

        GetNodeReport(env->isolate(),
                      env,
                      "Worker thread subreport",
                      trigger,
                      Local<Object>(),
                      os);

        Mutex::ScopedLock lock(workers_mutex);
        worker_infos.emplace_back(os.str());
        notify.Signal(lock);
      });
    });

    Mutex::ScopedLock lock(workers_mutex);
    worker_infos.reserve(expected_results);
    while (worker_infos.size() < expected_results)
      notify.Wait(lock);
    for (const std::string& worker_info : worker_infos)
      writer.json_element(JSONWriter::ForeignJSON { worker_info });
  }
  writer.json_arrayend();

  // Report operating system information
  PrintSystemInformation(&writer);

  writer.json_objectend();

  // Restore output stream formatting.
  out.copyfmt(old_state);
}

// Report Node.js version, OS version and machine information.
static void PrintVersionInformation(JSONWriter* writer) {
  std::ostringstream buf;
  // Report Node version
  buf << "v" << NODE_VERSION_STRING;
  writer->json_keyvalue("nodejsVersion", buf.str());
  buf.str("");

#ifndef _WIN32
  // Report compiler and runtime glibc versions where possible.
  const char* (*libc_version)();
  *(reinterpret_cast<void**>(&libc_version)) =
      dlsym(RTLD_DEFAULT, "gnu_get_libc_version");
  if (libc_version != nullptr)
    writer->json_keyvalue("glibcVersionRuntime", (*libc_version)());
#endif /* _WIN32 */

#ifdef __GLIBC__
  buf << __GLIBC__ << "." << __GLIBC_MINOR__;
  writer->json_keyvalue("glibcVersionCompiler", buf.str());
  buf.str("");
#endif

  // Report Process word size
  writer->json_keyvalue("wordSize", sizeof(void*) * 8);
  writer->json_keyvalue("arch", node::per_process::metadata.arch);
  writer->json_keyvalue("platform", node::per_process::metadata.platform);

  // Report deps component versions
  PrintComponentVersions(writer);

  // Report release metadata.
  PrintRelease(writer);

  // Report operating system and machine information
  uv_utsname_t os_info;

  if (uv_os_uname(&os_info) == 0) {
    writer->json_keyvalue("osName", os_info.sysname);
    writer->json_keyvalue("osRelease", os_info.release);
    writer->json_keyvalue("osVersion", os_info.version);
    writer->json_keyvalue("osMachine", os_info.machine);
  }

  PrintCpuInfo(writer);
  PrintNetworkInterfaceInfo(writer);

  char host[UV_MAXHOSTNAMESIZE];
  size_t host_size = sizeof(host);

  if (uv_os_gethostname(host, &host_size) == 0)
    writer->json_keyvalue("host", host);
}

// Report CPU info
static void PrintCpuInfo(JSONWriter* writer) {
  uv_cpu_info_t* cpu_info;
  int count;
  if (uv_cpu_info(&cpu_info, &count) == 0) {
    writer->json_arraystart("cpus");
    for (int i = 0; i < count; i++) {
      writer->json_start();
      writer->json_keyvalue("model", cpu_info[i].model);
      writer->json_keyvalue("speed", cpu_info[i].speed);
      writer->json_keyvalue("user", cpu_info[i].cpu_times.user);
      writer->json_keyvalue("nice", cpu_info[i].cpu_times.nice);
      writer->json_keyvalue("sys", cpu_info[i].cpu_times.sys);
      writer->json_keyvalue("idle", cpu_info[i].cpu_times.idle);
      writer->json_keyvalue("irq", cpu_info[i].cpu_times.irq);
      writer->json_end();
    }
    writer->json_arrayend();
    uv_free_cpu_info(cpu_info, count);
  }
}

static void PrintNetworkInterfaceInfo(JSONWriter* writer) {
  uv_interface_address_t* interfaces;
  char ip[INET6_ADDRSTRLEN];
  char netmask[INET6_ADDRSTRLEN];
  char mac[18];
  int count;

  if (uv_interface_addresses(&interfaces, &count) == 0) {
    writer->json_arraystart("networkInterfaces");

    for (int i = 0; i < count; i++) {
      writer->json_start();
      writer->json_keyvalue("name", interfaces[i].name);
      writer->json_keyvalue("internal", !!interfaces[i].is_internal);
      snprintf(mac,
               sizeof(mac),
               "%02x:%02x:%02x:%02x:%02x:%02x",
               static_cast<unsigned char>(interfaces[i].phys_addr[0]),
               static_cast<unsigned char>(interfaces[i].phys_addr[1]),
               static_cast<unsigned char>(interfaces[i].phys_addr[2]),
               static_cast<unsigned char>(interfaces[i].phys_addr[3]),
               static_cast<unsigned char>(interfaces[i].phys_addr[4]),
               static_cast<unsigned char>(interfaces[i].phys_addr[5]));
      writer->json_keyvalue("mac", mac);

      if (interfaces[i].address.address4.sin_family == AF_INET) {
        uv_ip4_name(&interfaces[i].address.address4, ip, sizeof(ip));
        uv_ip4_name(&interfaces[i].netmask.netmask4, netmask, sizeof(netmask));
        writer->json_keyvalue("address", ip);
        writer->json_keyvalue("netmask", netmask);
        writer->json_keyvalue("family", "IPv4");
      } else if (interfaces[i].address.address4.sin_family == AF_INET6) {
        uv_ip6_name(&interfaces[i].address.address6, ip, sizeof(ip));
        uv_ip6_name(&interfaces[i].netmask.netmask6, netmask, sizeof(netmask));
        writer->json_keyvalue("address", ip);
        writer->json_keyvalue("netmask", netmask);
        writer->json_keyvalue("family", "IPv6");
        writer->json_keyvalue("scopeid",
                              interfaces[i].address.address6.sin6_scope_id);
      } else {
        writer->json_keyvalue("family", "unknown");
      }

      writer->json_end();
    }

    writer->json_arrayend();
    uv_free_interface_addresses(interfaces, count);
  }
}

static void PrintJavaScriptErrorProperties(JSONWriter* writer,
                                           Isolate* isolate,
                                           Local<Object> error) {
  writer->json_objectstart("errorProperties");
  if (!error.IsEmpty()) {
    TryCatch try_catch(isolate);
    Local<Context> context = error->GetIsolate()->GetCurrentContext();
    Local<Array> keys;
    if (!error->GetOwnPropertyNames(context).ToLocal(&keys)) {
      return writer->json_objectend();  // the end of 'errorProperties'
    }
    uint32_t keys_length = keys->Length();
    for (uint32_t i = 0; i < keys_length; i++) {
      Local<Value> key;
      if (!keys->Get(context, i).ToLocal(&key) || !key->IsString()) {
        continue;
      }
      Local<Value> value;
      Local<String> value_string;
      if (!error->Get(context, key).ToLocal(&value) ||
          !value->ToString(context).ToLocal(&value_string)) {
        continue;
      }
      String::Utf8Value k(isolate, key);
      if (!strcmp(*k, "stack") || !strcmp(*k, "message")) continue;
      String::Utf8Value v(isolate, value_string);
      writer->json_keyvalue(std::string(*k, k.length()),
                            std::string(*v, v.length()));
    }
  }
  writer->json_objectend();  // the end of 'errorProperties'
}

// Report the JavaScript stack.
static void PrintJavaScriptErrorStack(JSONWriter* writer,
                                 Isolate* isolate,
                                 Local<Object> error,
                                 const char* trigger) {
  Local<Value> stackstr;
  std::string ss = "";
  TryCatch try_catch(isolate);
  if ((!strcmp(trigger, "FatalError")) ||
      (!strcmp(trigger, "Signal"))) {
    ss = "No stack.\nUnavailable.\n";
  } else if (!error.IsEmpty() &&
             error
                 ->Get(isolate->GetCurrentContext(),
                       node::FIXED_ONE_BYTE_STRING(isolate,
                                                   "stack"))
                 .ToLocal(&stackstr)) {
    String::Utf8Value sv(isolate, stackstr);
    ss = std::string(*sv, sv.length());
  }
  int line = ss.find('\n');
  if (line == -1) {
    writer->json_keyvalue("message", ss);
  } else {
    std::string l = ss.substr(0, line);
    writer->json_keyvalue("message", l);
    writer->json_arraystart("stack");
    ss = ss.substr(line + 1);
    line = ss.find('\n');
    while (line != -1) {
      l = ss.substr(0, line);
      l.erase(l.begin(), std::find_if(l.begin(), l.end(), [](int ch) {
                return !std::iswspace(ch);
              }));
      writer->json_element(l);
      ss = ss.substr(line + 1);
      line = ss.find('\n');
    }
    writer->json_arrayend();
  }
}

// Report a native stack backtrace
static void PrintNativeStack(JSONWriter* writer) {
  auto sym_ctx = NativeSymbolDebuggingContext::New();
  void* frames[256];
  const int size = sym_ctx->GetStackTrace(frames, arraysize(frames));
  writer->json_arraystart("nativeStack");
  int i;
  for (i = 1; i < size; i++) {
    void* frame = frames[i];
    writer->json_start();
    writer->json_keyvalue("pc",
                          ValueToHexString(reinterpret_cast<uintptr_t>(frame)));
    writer->json_keyvalue("symbol", sym_ctx->LookupSymbol(frame).Display());
    writer->json_end();
  }
  writer->json_arrayend();
}

// Report V8 JavaScript heap information.
// This uses the existing V8 HeapStatistics and HeapSpaceStatistics APIs.
// The isolate->GetGCStatistics(&heap_stats) internal V8 API could potentially
// provide some more useful information - the GC history and the handle counts
static void PrintGCStatistics(JSONWriter* writer, Isolate* isolate) {
  HeapStatistics v8_heap_stats;
  isolate->GetHeapStatistics(&v8_heap_stats);
  HeapSpaceStatistics v8_heap_space_stats;

  writer->json_objectstart("javascriptHeap");
  writer->json_keyvalue("totalMemory", v8_heap_stats.total_heap_size());
  writer->json_keyvalue("totalCommittedMemory",
                        v8_heap_stats.total_physical_size());
  writer->json_keyvalue("usedMemory", v8_heap_stats.used_heap_size());
  writer->json_keyvalue("availableMemory",
                        v8_heap_stats.total_available_size());
  writer->json_keyvalue("memoryLimit", v8_heap_stats.heap_size_limit());

  writer->json_objectstart("heapSpaces");
  // Loop through heap spaces
  for (size_t i = 0; i < isolate->NumberOfHeapSpaces(); i++) {
    isolate->GetHeapSpaceStatistics(&v8_heap_space_stats, i);
    writer->json_objectstart(v8_heap_space_stats.space_name());
    writer->json_keyvalue("memorySize", v8_heap_space_stats.space_size());
    writer->json_keyvalue(
        "committedMemory",
        v8_heap_space_stats.physical_space_size());
    writer->json_keyvalue(
        "capacity",
        v8_heap_space_stats.space_used_size() +
            v8_heap_space_stats.space_available_size());
    writer->json_keyvalue("used", v8_heap_space_stats.space_used_size());
    writer->json_keyvalue(
        "available", v8_heap_space_stats.space_available_size());
    writer->json_objectend();
  }

  writer->json_objectend();
  writer->json_objectend();
}

static void PrintResourceUsage(JSONWriter* writer) {
  // Get process uptime in seconds
  uint64_t uptime =
      (uv_hrtime() - node::per_process::node_start_time) / (NANOS_PER_SEC);
  if (uptime == 0) uptime = 1;  // avoid division by zero.

  // Process and current thread usage statistics
  uv_rusage_t rusage;
  writer->json_objectstart("resourceUsage");
  if (uv_getrusage(&rusage) == 0) {
    double user_cpu =
        rusage.ru_utime.tv_sec + SEC_PER_MICROS * rusage.ru_utime.tv_usec;
    double kernel_cpu =
        rusage.ru_stime.tv_sec + SEC_PER_MICROS * rusage.ru_stime.tv_usec;
    writer->json_keyvalue("userCpuSeconds", user_cpu);
    writer->json_keyvalue("kernelCpuSeconds", kernel_cpu);
    double cpu_abs = user_cpu + kernel_cpu;
    double cpu_percentage = (cpu_abs / uptime) * 100.0;
    writer->json_keyvalue("cpuConsumptionPercent", cpu_percentage);
    writer->json_keyvalue("maxRss", rusage.ru_maxrss * 1024);
    writer->json_objectstart("pageFaults");
    writer->json_keyvalue("IORequired", rusage.ru_majflt);
    writer->json_keyvalue("IONotRequired", rusage.ru_minflt);
    writer->json_objectend();
    writer->json_objectstart("fsActivity");
    writer->json_keyvalue("reads", rusage.ru_inblock);
    writer->json_keyvalue("writes", rusage.ru_oublock);
    writer->json_objectend();
  }
  writer->json_objectend();
#ifdef RUSAGE_THREAD
  struct rusage stats;
  if (getrusage(RUSAGE_THREAD, &stats) == 0) {
    writer->json_objectstart("uvthreadResourceUsage");
    double user_cpu =
        stats.ru_utime.tv_sec + SEC_PER_MICROS * stats.ru_utime.tv_usec;
    double kernel_cpu =
        stats.ru_stime.tv_sec + SEC_PER_MICROS * stats.ru_stime.tv_usec;
    writer->json_keyvalue("userCpuSeconds", user_cpu);
    writer->json_keyvalue("kernelCpuSeconds", kernel_cpu);
    double cpu_abs = user_cpu + kernel_cpu;
    double cpu_percentage = (cpu_abs / uptime) * 100.0;
    writer->json_keyvalue("cpuConsumptionPercent", cpu_percentage);
    writer->json_objectstart("fsActivity");
    writer->json_keyvalue("reads", stats.ru_inblock);
    writer->json_keyvalue("writes", stats.ru_oublock);
    writer->json_objectend();
    writer->json_objectend();
  }
#endif
}

// Report operating system information.
static void PrintSystemInformation(JSONWriter* writer) {
  uv_env_item_t* envitems;
  int envcount;
  int r;

  writer->json_objectstart("environmentVariables");

  {
    Mutex::ScopedLock lock(node::per_process::env_var_mutex);
    r = uv_os_environ(&envitems, &envcount);
  }

  if (r == 0) {
    for (int i = 0; i < envcount; i++)
      writer->json_keyvalue(envitems[i].name, envitems[i].value);

    uv_os_free_environ(envitems, envcount);
  }

  writer->json_objectend();

#ifndef _WIN32
  static struct {
    const char* description;
    int id;
  } rlimit_strings[] = {
    {"core_file_size_blocks", RLIMIT_CORE},
    {"data_seg_size_kbytes", RLIMIT_DATA},
    {"file_size_blocks", RLIMIT_FSIZE},
#if !(defined(_AIX) || defined(__sun))
    {"max_locked_memory_bytes", RLIMIT_MEMLOCK},
#endif
#ifndef __sun
    {"max_memory_size_kbytes", RLIMIT_RSS},
#endif
    {"open_files", RLIMIT_NOFILE},
    {"stack_size_bytes", RLIMIT_STACK},
    {"cpu_time_seconds", RLIMIT_CPU},
#ifndef __sun
    {"max_user_processes", RLIMIT_NPROC},
#endif
#ifndef __OpenBSD__
    {"virtual_memory_kbytes", RLIMIT_AS}
#endif
  };

  writer->json_objectstart("userLimits");
  struct rlimit limit;
  std::string soft, hard;

  for (size_t i = 0; i < arraysize(rlimit_strings); i++) {
    if (getrlimit(rlimit_strings[i].id, &limit) == 0) {
      writer->json_objectstart(rlimit_strings[i].description);

      if (limit.rlim_cur == RLIM_INFINITY)
        writer->json_keyvalue("soft", "unlimited");
      else
        writer->json_keyvalue("soft", limit.rlim_cur);

      if (limit.rlim_max == RLIM_INFINITY)
        writer->json_keyvalue("hard", "unlimited");
      else
        writer->json_keyvalue("hard", limit.rlim_max);

      writer->json_objectend();
    }
  }
  writer->json_objectend();
#endif  // _WIN32

  PrintLoadedLibraries(writer);
}

// Report a list of loaded native libraries.
static void PrintLoadedLibraries(JSONWriter* writer) {
  writer->json_arraystart("sharedObjects");
  std::vector<std::string> modules =
      NativeSymbolDebuggingContext::GetLoadedLibraries();
  for (auto const& module_name : modules) writer->json_element(module_name);
  writer->json_arrayend();
}

// Obtain and report the node and subcomponent version strings.
static void PrintComponentVersions(JSONWriter* writer) {
  std::stringstream buf;

  writer->json_objectstart("componentVersions");

#define V(key)                                                                 \
  writer->json_keyvalue(#key, node::per_process::metadata.versions.key);
  NODE_VERSIONS_KEYS(V)
#undef V

  writer->json_objectend();
}

// Report runtime release information.
static void PrintRelease(JSONWriter* writer) {
  writer->json_objectstart("release");
  writer->json_keyvalue("name", node::per_process::metadata.release.name);
#if NODE_VERSION_IS_LTS
  writer->json_keyvalue("lts", node::per_process::metadata.release.lts);
#endif

#ifdef NODE_HAS_RELEASE_URLS
  writer->json_keyvalue("headersUrl",
                        node::per_process::metadata.release.headers_url);
  writer->json_keyvalue("sourceUrl",
                        node::per_process::metadata.release.source_url);
#ifdef _WIN32
  writer->json_keyvalue("libUrl", node::per_process::metadata.release.lib_url);
#endif  // _WIN32
#endif  // NODE_HAS_RELEASE_URLS

  writer->json_objectend();
}

}  // namespace report
