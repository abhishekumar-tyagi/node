#ifndef SRC_INSPECTOR_WORKER_INSPECTOR_H_
#define SRC_INSPECTOR_WORKER_INSPECTOR_H_

#if !HAVE_INSPECTOR
#error("This header can only be used when inspector is enabled")
#endif

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace node {
namespace inspector {
class MainThreadHandle;
class WorkerManager;

class WorkerDelegate {
 public:
  virtual void WorkerCreated(const std::string& title,
                             const std::string& url,
                             bool waiting,
                             std::shared_ptr<MainThreadHandle> worker) = 0;
  virtual ~WorkerDelegate() = default;
};

class WorkerManagerEventHandle {
 public:
  explicit WorkerManagerEventHandle(std::shared_ptr<WorkerManager> manager,
                                    int id)
                                    : manager_(manager), id_(id) {}
  void SetWaitOnStart(bool wait_on_start);
  ~WorkerManagerEventHandle();

 private:
  std::shared_ptr<WorkerManager> manager_;
  int id_;
};

struct WorkerInfo {
  WorkerInfo(const std::string& target_title,
             const std::string& target_url,
             std::shared_ptr<MainThreadHandle> worker_thread)
             : title(target_title),
               url(target_url),
               worker_thread(worker_thread) {}
  std::string title;
  std::string url;
  std::shared_ptr<MainThreadHandle> worker_thread;
};

class ParentInspectorHandle {
 public:
  ParentInspectorHandle(int id, const std::string& url,
                        std::shared_ptr<MainThreadHandle> parent_thread,
                        bool wait_for_connect);
  ~ParentInspectorHandle();
  void WorkerStarted(std::shared_ptr<MainThreadHandle> worker_thread,
                     bool waiting);
  bool WaitForConnect() {
    return wait_;
  }

 private:
  int id_;
  std::string url_;
  std::shared_ptr<MainThreadHandle> parent_thread_;
  bool wait_;
};

class WorkerManager : public std::enable_shared_from_this<WorkerManager> {
 public:
  explicit WorkerManager(std::shared_ptr<MainThreadHandle> thread)
                         : thread_(thread) {}

  std::unique_ptr<ParentInspectorHandle> NewParentHandle(
      int thread_id, const std::string& url);
  void WorkerStarted(int session_id, const WorkerInfo& info, bool waiting);
  void WorkerFinished(int session_id);
  std::unique_ptr<WorkerManagerEventHandle> SetAutoAttach(
      std::unique_ptr<WorkerDelegate> attach_delegate);
  void SetWaitOnStartForDelegate(int id, bool wait);
  void RemoveAttachDelegate(int id);
  std::shared_ptr<MainThreadHandle> MainThread() {
    return thread_;
  }

 private:
  std::shared_ptr<MainThreadHandle> thread_;
  std::unordered_map<int, WorkerInfo> children_;
  std::unordered_map<int, std::unique_ptr<WorkerDelegate>> delegates_;
  // If any one needs it, workers stop for all
  std::unordered_set<int> delegates_waiting_on_start_;
  int next_delegate_id_ = 0;
};
}  // namespace inspector
}  // namespace node

#endif  // SRC_INSPECTOR_WORKER_INSPECTOR_H_
