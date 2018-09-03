#ifndef SRC_NODE_THREADPOOL_H_
#define SRC_NODE_THREADPOOL_H_

#if defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS

#include <queue>
#include <vector>
#include <functional>

#include "node.h"
#include "node_mutex.h"
#include "uv.h"

namespace node {
namespace threadpool {

class Threadpool;
class TaskQueue;
class Task;
class TaskDetails;
class Worker;

// Inhabited by a uv_thread_t.
// Subclass to experiment, e.g.:
//   - cancellation (a la Davis et al. 2018's Manager-Worker-Hangman approach)
class Worker {
 public:
  Worker();

  // Starts a thread and returns control to the caller.
  void Start(TaskQueue* queue);
  void Join(void);

 protected:
  // Override e.g. to implement cancellation.
  static void _Run(void* data);

  uv_thread_t self_;

 private:
};

// This is basically a struct
class TaskDetails {
 public:
  enum TaskType {
      FS
    , FS_LIKELY_CACHED  // Likely to be bound by memory or CPU
    , OTHER_DISK_IO
    , DNS
    , OTHER_NETWORK_IO
    , IO
    , MEMORY
    , CPU
    , CPU_SLOW
    , CPU_FAST
    , UNKNOWN
  };

  TaskType type;
  int priority;  // Larger numbers signal higher priority.i
                 // Does nothing in this class.
  bool cancelable;  // If true, by some yet-to-be-determined mechanism we can
                    // cancel this Task while it is scheduled.

 protected:
 private:
};

// Abstract notion of a Task.
// Clients of node::Threadpool should sub-class this for their type of request.
//  - V8::Platform Tasks
//  - libuv async work
//  - User work from the N-API
class Task {
 public:
  Task() {}
  // Invoked after Run().
  virtual ~Task() {}

  // Invoked on some thread in the Threadpool.
  virtual void Run() = 0;

  // Run() can access details.
  // Should be set in subclass constructor.
  TaskDetails details_;
 private:
};

class LibuvTask : public Task {
 public:
  LibuvTask(Threadpool* tp, uv_work_t* req, const uv_work_options_t* opts);
  ~LibuvTask();

  void Run();

 protected:
 private:
  Threadpool* tp_;
  uv_work_t* req_;
};

// Abstract notion of a queue of Tasks.
// The default implementation is a FIFO queue.
// Subclass to experiment, e.g.:
//   - prioritization
//   - multi-queue e.g. for CPU-bound and I/O-bound Tasks or Fast and Slow ones.
class TaskQueue {
 public:
  TaskQueue();

  // Return true if Push succeeds, else false.
  bool Push(std::unique_ptr<Task> task);
  std::unique_ptr<Task> Pop(void);

  // Returns nullptr when we're done.
  std::unique_ptr<Task> BlockingPop(void);

  // Subsequent Push() will fail.
  // Pop calls will return nullptr once queue is drained.
  void Stop();

  int Length(void) const;

 private:
  // Structures.
  std::queue<std::unique_ptr<Task>> queue_;
  bool stopped_;

  // Synchronization.
  Mutex lock_;
  ConditionVariable tasks_available_;
};

// A threadpool works on asynchronous Tasks.
// It consists of:
//   - a TaskQueue of pending Tasks
//   - a set of Workers that handle Tasks from the TaskQueue
// Subclass to experiment, e.g.:
//   - Use a different type of TaskQueue
//   - Elastic workers (scale up and down)
class Threadpool {
 public:
  Threadpool(void);
  // Waits for queue to drain.
  ~Threadpool(void);

  // Call once, before you Post.
  // TODO(davisjam): RAII?
  void Initialize(void);
  // TODO(davisjam): Destroy.

  void Post(std::unique_ptr<Task> task);
  int QueueLength(void) const;

  // To interact with libuv's executor API:
  //   - For the call to uv_replace_executor
  //   - A LibuvTask needs the uv_executor_done_cb
  uv_executor_t* GetExecutor();

 protected:
  // TODO(davisjam): This should be in some separate interface class like
  //   NodePlatform::WorkerThreadsTaskRunner.
  uv_executor_t executor_;  // So can be plugged in to libuv
  static void uv_executor_init(uv_executor_t* executor);
  static void uv_executor_submit(uv_executor_t* executor,
                                 uv_work_t* req,
                                 const uv_work_options_t* opts);

 private:
  TaskQueue queue_;
  std::vector<std::unique_ptr<Worker>> workers_;
};

}  // namespace threadpool
}  // namespace node

#endif  // defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS

#endif  // SRC_NODE_THREADPOOL_H_
