#ifndef SRC_INSPECTOR_IO_H_
#define SRC_INSPECTOR_IO_H_

#include "inspector_socket_server.h"
#include "node_debug_options.h"
#include "node_mutex.h"
#include "uv.h"

#include <stddef.h>
#include <memory>

#if !HAVE_INSPECTOR
#error("This header can only be used when inspector is enabled")
#endif

namespace v8_inspector {
class StringBuffer;
class StringView;
}  // namespace v8_inspector

namespace node {
// Forward declaration to break recursive dependency chain with src/env.h.
class Environment;
namespace inspector {

std::string FormatWsAddress(const std::string& host,
                            int port,
                            const std::string& target_id,
                            bool include_protocol);

class InspectorIoDelegate;
class MainThreadHandle;
class RequestQueue;

// kKill closes connections and stops the server, kStop only stops the server
enum class TransportAction { kKill, kSendMessage, kStop };

class InspectorIo {
 public:
  // Start the inspector agent thread, waiting for it to initialize
  // bool Start();
  // Returns empty pointer if thread was not started
  static std::unique_ptr<InspectorIo> Start(
      std::shared_ptr<MainThreadHandle> main_thread,
      const std::string& path,
      const DebugOptions& options);

  // Will block till the transport thread shuts down
  ~InspectorIo();

  void StopAcceptingNewConnections();
  std::string host() const { return options_.host_name(); }
  int port() const { return port_; }
  std::vector<std::string> GetTargetIds() const;

 private:
  InspectorIo(std::shared_ptr<MainThreadHandle> handle,
              const std::string& path,
              const DebugOptions& options);

  // Wrapper for agent->ThreadMain()
  static void ThreadMain(void* agent);

  // Runs a uv_loop_t
  void ThreadMain();

  // This is a thread-safe object that will post async tasks. It lives as long
  // as an Inspector object lives (almost as long as an Isolate).
  std::shared_ptr<MainThreadHandle> main_thread_;
  // Used to post on a frontend interface thread, lives while the server is
  // running
  std::shared_ptr<RequestQueue> request_queue_;
  const DebugOptions options_;

  // The IO thread runs its own uv_loop to implement the TCP server off
  // the main thread.
  uv_thread_t thread_;

  // For setting up interthread communications
  Mutex thread_start_lock_;
  ConditionVariable thread_start_condition_;
  std::string script_name_;
  int port_ = -1;
  // May be accessed from any thread
  const std::string id_;
};

}  // namespace inspector
}  // namespace node

#endif  // SRC_INSPECTOR_IO_H_
