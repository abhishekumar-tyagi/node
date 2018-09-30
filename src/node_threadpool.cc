#include "node_threadpool.h"
#include "node_internals.h"

#include "env-inl.h"
#include "debug_utils.h"
#include "util.h"

#include <algorithm>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>


// TODO(davisjam): DO NOT MERGE. Only for debugging.
// TODO(davisjam): There must be a better way to do this.
#define DEBUG_LOG 1
// #undef DEBUG_LOG

#ifdef DEBUG_LOG
#define LOG_TO_FILE(fd, ...) fprintf(fd, __VA_ARGS__)
#define LOG(...) LOG_TO_FILE(stderr, __VA_ARGS__)
#else
#define LOG_TO_FILE(...) (void) 0
#define LOG(...) (void) 0
#endif

namespace node {
namespace threadpool {

/**************
 * NodeThreadpool
 ***************/

NodeThreadpool::NodeThreadpool() : tp_(nullptr) {
}

NodeThreadpool::NodeThreadpool(int threadpool_size) {
  if (threadpool_size <= 0) {
    // Check UV_THREADPOOL_SIZE
    char buf[32];
    size_t buf_size = sizeof(buf);
    if (uv_os_getenv("UV_THREADPOOL_SIZE", buf, &buf_size) == 0) {
      threadpool_size = atoi(buf);
    }
  }

  if (threadpool_size <= 0) {
    // No/bad UV_THREADPOOL_SIZE, so take a guess.
    threadpool_size = GoodCPUThreadpoolSize();
  }
  LOG("NodeThreadpool::NodeThreadpool: threadpool_size %d\n", threadpool_size);
  CHECK_GT(threadpool_size, 0);

  tp_ = std::make_shared<Threadpool>(threadpool_size, 0);
}

int NodeThreadpool::GoodCPUThreadpoolSize(void) {
  // Ask libuv how many cores we have.
  uv_cpu_info_t* cpu_infos;
  int cpu_count;

  if (uv_cpu_info(&cpu_infos, &cpu_count)) {
    LOG("NodeThreadpool::GoodCPUThreadpoolSize: Huh, uv_cpu_info failed?\n");
    return 4;  // Old libuv TP default.
  }

  uv_free_cpu_info(cpu_infos, cpu_count);
    LOG("NodeThreadpool::GoodCPUThreadpoolSize: cpu_count %d\n", cpu_count);
  return cpu_count - 1;  // Leave one core for main loop
}

NodeThreadpool::~NodeThreadpool() {
}

std::shared_ptr<TaskState> NodeThreadpool::Post(std::unique_ptr<Task> task) {
  return tp_->Post(std::move(task));
}

void NodeThreadpool::BlockingDrain() {
  return tp_->BlockingDrain();
}

int NodeThreadpool::QueueLength() const {
  return tp_->QueueLength();
}

int NodeThreadpool::NWorkers() const {
  return tp_->NWorkers();
}

void NodeThreadpool::PrintStats() const {
}

/**************
 * PartitionedNodeThreadpool
 ***************/

PartitionedNodeThreadpool::PartitionedNodeThreadpool() {
  LOG("PNT::PNT: default constructor\n");
}

PartitionedNodeThreadpool::PartitionedNodeThreadpool
(std::vector<int> tp_sizes) {
  LOG("PNT::PNT: vector constructor\n");
  Initialize(tp_sizes);
}

void PartitionedNodeThreadpool::Initialize(const std::vector<int>& tp_sizes) {
  int i = 0;
  for (auto size : tp_sizes) {
    LOG("PNT::Initialize: tp %d: %d threads\n", i, size);
    std::shared_ptr<Threadpool> tp = std::make_shared<Threadpool>(size, i);
    tps_.push_back(tp);
    i++;
  }
}

PartitionedNodeThreadpool::~PartitionedNodeThreadpool() {
  LOG("PNT::~PartitionedNodeThreadpool: Goodbye\n");
  fflush(stderr);

  // If we just return, the destructors of the tp's will drain them.
  // But if we want to report meaningful statistics we must drain them first.
  for (auto &tp : tps_) {
    tp->BlockingDrain();
  }
}

void PartitionedNodeThreadpool::PrintStats(void) const {
  // TODO(davisjam) Let's hope the application didn't
  // make more than one of these :-D.
  char logFile[128];
  snprintf(logFile, sizeof(logFile),
    "/tmp/node-%d-PartitionedNodeThreadpool-result.log", getpid());
  FILE* fd = fopen(logFile, "w");
  CHECK(fd);

  // Emit records in machine-parseable format as well as human-readable.
  // We want to be able to easily grep for and extract lines into data frames
  // so we can slice and dice quickly.
  LOG_TO_FILE(fd, "TP key format: TP,name,size\n");
  for (auto pair : tp_labels_) {
    LOG_TO_FILE(fd, "TP key: %d,%s,%d\n",
      pair.first, tp_labels_.at(pair.first).c_str(), tp_sizes_.at(pair.first));
  }

  LOG_TO_FILE(fd,
    "QueueLengths data format: TP,queue-length,n_cpu,n_io,time\n");
  LOG_TO_FILE(fd, "TaskSummary data format: TP,task-origin,task-type,"
                  "queue-duration,run-duration,time-at-completion\n");

  for (size_t i = 0; i < tps_.size(); i++) {
    auto &tp = tps_[i];
    LOG("Report on TP %d\n", tp->Id());

    const std::vector<std::unique_ptr<QueueLengthSample>> &lengths =
      tp->GetQueueLengths();
    LOG("  TP %d: Lengths at the %lu update intervals:\n",
      tp->Id(), lengths.size());

    if (lengths.size()) {
      for (const std::unique_ptr<QueueLengthSample> &length : lengths) {
        LOG("    TP %d length %d n-cpu %d n-io %d time %lu\n",
          tp->Id(), length->length_, length->n_cpu_,
          length->n_io_, length->time_);
        LOG_TO_FILE(fd, "QueueLengths data: %d,%d,%d,%d,%lu\n",
          tp->Id(), length->length_,
          length->n_cpu_, length->n_io_,
          length->time_);
      }
    }

    const std::vector<std::unique_ptr<TaskSummary>> &summaries =
      tp->GetTaskSummaries();
    LOG("  TP %d: Task summaries for the %lu tasks:\n",
      tp->Id(), summaries.size());
    for (const std::unique_ptr<TaskSummary> &summary : summaries) {
      LOG("    TP %d origin %s type %s queue-duration %lu"
          "run-duration %lu time-at-completion %lu\n",
        tp->Id(), TaskDetails::AsString(summary->details_.origin).c_str(),
        TaskDetails::AsString(summary->details_.type).c_str(),
        summary->time_in_queue_, summary->time_in_run_,
        summary->time_at_completion_);
      LOG_TO_FILE(fd, "TaskSummary data: %d,%s,%s,%lu,%lu,%lu\n",
        tp->Id(), TaskDetails::AsString(summary->details_.origin).c_str(),
        TaskDetails::AsString(summary->details_.type).c_str(),
        summary->time_in_queue_, summary->time_in_run_,
        summary->time_at_completion_);
    }
  }
}

std::shared_ptr<TaskState>
PartitionedNodeThreadpool::Post(std::unique_ptr<Task> task) {
  int tp = ChooseThreadpool(task.get());
  CHECK_GE(tp, 0);
  CHECK_LT(tp, (int) tps_.size());
  return tps_[tp]->Post(std::move(task));
}

void PartitionedNodeThreadpool::BlockingDrain() {
  for (auto &tp : tps_) {
    tp->BlockingDrain();
  }
}

int PartitionedNodeThreadpool::QueueLength() const {
  int sum = 0;
  for (auto &tp : tps_) {
    sum += tp->QueueLength();
  }
  return sum;
}

int PartitionedNodeThreadpool::NWorkers() const {
  int sum = 0;
  for (auto &tp : tps_) {
    sum += tp->NWorkers();
  }
  return sum;
}

/**************
 * UnpartitionedPartitionedNodeThreadpool
 ***************/

UnpartitionedPartitionedNodeThreadpool::UnpartitionedPartitionedNodeThreadpool(
  std::vector<int> tp_sizes) : ONLY_TP_IX(0) {
  CHECK_EQ(tp_sizes.size(), 1);

  // TP size
  if (tp_sizes[ONLY_TP_IX] <= 0) {
    char buf[32];
    size_t buf_size = sizeof(buf);
    if (uv_os_getenv("UV_THREADPOOL_SIZE", buf, &buf_size) == 0) {
      tp_sizes[ONLY_TP_IX] = atoi(buf);
    }
  }
  if (tp_sizes[ONLY_TP_IX] <= 0) {
    tp_sizes[ONLY_TP_IX] = 4;  // libuv default
  }
  LOG("UPNT::UPNT: only tp size %d\n", tp_sizes[ONLY_TP_IX]);
  CHECK_GT(tp_sizes[ONLY_TP_IX], 0);

  Initialize(tp_sizes);

  tp_labels_[ONLY_TP_IX] = "Universal TP";
  tp_sizes_[ONLY_TP_IX] = tp_sizes[ONLY_TP_IX];
}

UnpartitionedPartitionedNodeThreadpool::
~UnpartitionedPartitionedNodeThreadpool() {
}

int UnpartitionedPartitionedNodeThreadpool::ChooseThreadpool(Task* task) const {
  return ONLY_TP_IX;
}

/**************
 * ByTaskOriginPartitionedNodeThreadpool
 ***************/

ByTaskOriginPartitionedNodeThreadpool::ByTaskOriginPartitionedNodeThreadpool(
  std::vector<int> tp_sizes) : V8_TP_IX(0), LIBUV_TP_IX(1) {
  CHECK_EQ(tp_sizes.size(), 2);

  // V8 TP size
  if (tp_sizes[V8_TP_IX] <= 0) {
    char buf[32];
    size_t buf_size = sizeof(buf);
    if (uv_os_getenv("NODE_THREADPOOL_V8_TP_SIZE", buf, &buf_size) == 0) {
      tp_sizes[V8_TP_IX] = atoi(buf);
    }
  }
  if (tp_sizes[V8_TP_IX] <= 0) {
    // No/bad env var, so take a guess.
    tp_sizes[V8_TP_IX] = GoodCPUThreadpoolSize();
  }
  LOG("BTOPNT::BTOPNT: v8 tp size %d\n", tp_sizes[V8_TP_IX]);
  CHECK_GT(tp_sizes[V8_TP_IX], 0);

  // LIBUV TP size
  if (tp_sizes[LIBUV_TP_IX] <= 0) {
    char buf[32];
    size_t buf_size = sizeof(buf);
    if (uv_os_getenv("UV_THREADPOOL_SIZE", buf, &buf_size) == 0) {
      tp_sizes[LIBUV_TP_IX] = atoi(buf);
    } else if (uv_os_getenv("NODE_THREADPOOL_UV_TP_SIZE", buf, &buf_size)
        == 0) {
      tp_sizes[LIBUV_TP_IX] = atoi(buf);
    }
  }
  if (tp_sizes[LIBUV_TP_IX] <= 0) {
    tp_sizes[LIBUV_TP_IX] = 1 * tp_sizes[V8_TP_IX];
  }
  LOG("BTOPNT::BTOPNT: libuv tp size %d\n", tp_sizes[LIBUV_TP_IX]);
  CHECK_GT(tp_sizes[LIBUV_TP_IX], 0);

  Initialize(tp_sizes);

  tp_labels_[V8_TP_IX] = "V8 TP";
  tp_sizes_[V8_TP_IX] = tp_sizes[V8_TP_IX];
  tp_labels_[LIBUV_TP_IX] = "Libuv TP";
  tp_sizes_[LIBUV_TP_IX] = tp_sizes[LIBUV_TP_IX];
}

ByTaskOriginPartitionedNodeThreadpool::
~ByTaskOriginPartitionedNodeThreadpool() {
}

int ByTaskOriginPartitionedNodeThreadpool::ChooseThreadpool(Task* task) const {
  switch (task->details_.origin) {
    case TaskDetails::V8:
      LOG("BTOPNT::ChooseThreadpool: V8\n");
      return V8_TP_IX;
    default:
      LOG("BTOPNT::ChooseThreadpool: LIBUV\n");
      return LIBUV_TP_IX;
  }
}

/**************
 * ByTaskTypePartitionedNodeThreadpool
 ***************/

ByTaskTypePartitionedNodeThreadpool::ByTaskTypePartitionedNodeThreadpool(
  std::vector<int> tp_sizes) : CPU_TP_IX(0), IO_TP_IX(1) {
  CHECK_EQ(tp_sizes.size(), 2);

  // CPU TP size
  if (tp_sizes[CPU_TP_IX] <= 0) {
    char buf[32];
    size_t buf_size = sizeof(buf);
    if (uv_os_getenv("NODE_THREADPOOL_CPU_TP_SIZE", buf, &buf_size) == 0) {
      tp_sizes[CPU_TP_IX] = atoi(buf);
    }
  }
  if (tp_sizes[CPU_TP_IX] <= 0) {
    // No/bad env var, so take a guess.
    tp_sizes[CPU_TP_IX] = GoodCPUThreadpoolSize();
  }
  LOG("BTTPNT::BTTPNT: cpu_pool_size %d\n", tp_sizes[CPU_TP_IX]);
  CHECK_GT(tp_sizes[CPU_TP_IX], 0);

  // IO TP size
  if (tp_sizes[IO_TP_IX] <= 0) {
    char buf[32];
    size_t buf_size = sizeof(buf);
    if (uv_os_getenv("NODE_THREADPOOL_IO_TP_SIZE", buf, &buf_size) == 0) {
      tp_sizes[IO_TP_IX] = atoi(buf);
    }
  }
  if (tp_sizes[IO_TP_IX] < 0) {
    tp_sizes[IO_TP_IX] = 1 * tp_sizes[CPU_TP_IX];
  }
  LOG("BTTPNT::BTTPNT: io_pool_size %d\n", tp_sizes[IO_TP_IX]);
  CHECK_GT(tp_sizes[IO_TP_IX], 0);

  Initialize(tp_sizes);

  tp_labels_[CPU_TP_IX] = "CPU TP";
  tp_sizes_[CPU_TP_IX] = tp_sizes[CPU_TP_IX];
  tp_labels_[IO_TP_IX] = "IO TP";
  tp_sizes_[IO_TP_IX] = tp_sizes[IO_TP_IX];
}

ByTaskTypePartitionedNodeThreadpool::~ByTaskTypePartitionedNodeThreadpool() {
}

int ByTaskTypePartitionedNodeThreadpool::ChooseThreadpool(Task* task) const {
  switch (task->details_.type) {
    case TaskDetails::CPU:
    case TaskDetails::MEMORY:
    case TaskDetails::TASK_TYPE_UNKNOWN:
      LOG("BTTPNT::ChooseThreadpool: CPU\n");
      return CPU_TP_IX;
    default:
      LOG("BTTPNT::ChooseThreadpool: IO\n");
      return IO_TP_IX;
  }
}

/**************
 * ByTaskOriginAndTypePartitionedNodeThreadpool
 ***************/

ByTaskOriginAndTypePartitionedNodeThreadpool::
ByTaskOriginAndTypePartitionedNodeThreadpool(
  std::vector<int> tp_sizes)
  : V8_TP_IX(0), LIBUV_CPU_TP_IX(1), LIBUV_IO_TP_IX(2) {
  CHECK_EQ(tp_sizes.size(), 3);

  // V8 TP size
  if (tp_sizes[V8_TP_IX] <= 0) {
    char buf[32];
    size_t buf_size = sizeof(buf);
    if (uv_os_getenv("NODE_THREADPOOL_V8_TP_SIZE", buf, &buf_size) == 0) {
      tp_sizes[V8_TP_IX] = atoi(buf);
    }
  }
  if (tp_sizes[V8_TP_IX] <= 0) {
    // No/bad env var, so take a guess.
    tp_sizes[V8_TP_IX] = GoodCPUThreadpoolSize();
  }
  LOG("BTOATPNT::BTOATPNT: v8 tp size %d\n", tp_sizes[V8_TP_IX]);
  CHECK_GT(tp_sizes[V8_TP_IX], 0);

  // LIBUV-CPU TP size
  if (tp_sizes[LIBUV_CPU_TP_IX] <= 0) {
    char buf[32];
    size_t buf_size = sizeof(buf);
    if (uv_os_getenv("NODE_THREADPOOL_UV_CPU_TP_SIZE", buf, &buf_size) == 0) {
      tp_sizes[LIBUV_CPU_TP_IX] = atoi(buf);
    }
  }
  if (tp_sizes[LIBUV_CPU_TP_IX] <= 0) {
    // No/bad env var, so take a guess.
    tp_sizes[LIBUV_CPU_TP_IX] = GoodCPUThreadpoolSize();
  }
  LOG("BTOATPNT::BTOATPNT: libuv cpu pool size %d\n",
    tp_sizes[LIBUV_CPU_TP_IX]);
  CHECK_GT(tp_sizes[LIBUV_CPU_TP_IX], 0);

  // IO TP size
  if (tp_sizes[LIBUV_IO_TP_IX] <= 0) {
    char buf[32];
    size_t buf_size = sizeof(buf);
    if (uv_os_getenv("NODE_THREADPOOL_UV_IO_TP_SIZE", buf, &buf_size) == 0) {
      tp_sizes[LIBUV_IO_TP_IX] = atoi(buf);
    }
  }
  if (tp_sizes[LIBUV_IO_TP_IX] < 0) {
    tp_sizes[LIBUV_IO_TP_IX] = 1 * tp_sizes[LIBUV_CPU_TP_IX];
  }
  LOG("BTOATPNT::BTOATPNT: libuv io pool size %d\n", tp_sizes[LIBUV_IO_TP_IX]);
  CHECK_GT(tp_sizes[LIBUV_IO_TP_IX], 0);

  Initialize(tp_sizes);

  tp_labels_[V8_TP_IX] = "V8 TP";
  tp_sizes_[V8_TP_IX] = tp_sizes[V8_TP_IX];
  tp_labels_[LIBUV_CPU_TP_IX] = "Libuv CPU TP";
  tp_sizes_[LIBUV_CPU_TP_IX] = tp_sizes[LIBUV_CPU_TP_IX];
  tp_labels_[LIBUV_IO_TP_IX] = "Libuv IO TP";
  tp_sizes_[LIBUV_IO_TP_IX] = tp_sizes[LIBUV_IO_TP_IX];
}

ByTaskOriginAndTypePartitionedNodeThreadpool::
~ByTaskOriginAndTypePartitionedNodeThreadpool() {
}

int
ByTaskOriginAndTypePartitionedNodeThreadpool::ChooseThreadpool(Task* task)
const {
  if (task->details_.origin == TaskDetails::V8) {
    LOG("BTOATPNT::ChooseThreadpool: V8\n");
    return V8_TP_IX;
  } else if (task->details_.origin == TaskDetails::LIBUV) {
    switch (task->details_.type) {
      case TaskDetails::CPU:
      case TaskDetails::MEMORY:
      case TaskDetails::TASK_TYPE_UNKNOWN:
        LOG("BTOATPNT::ChooseThreadpool: CPU\n");
        return LIBUV_CPU_TP_IX;
      default:
        LOG("BTOATPNT::ChooseThreadpool:\ I/O\n");
        return LIBUV_IO_TP_IX;
    }
  } else {
    LOG("BTOATPNT::ChooseThreadpool: Unexpected origin %d --> libuv I/O pool\n",
      task->details_.origin);
    return LIBUV_IO_TP_IX;
  }
}

/**************
 * WorkerGroup
 ***************/

WorkerGroup::WorkerGroup(int n_workers, std::shared_ptr<TaskQueue> tq)
  : workers_() {
  for (int i = 0; i < n_workers; i++) {
    std::unique_ptr<Worker> worker(new Worker(tq));
    worker->Start();
    workers_.push_back(std::move(worker));
  }
}

WorkerGroup::~WorkerGroup() {
  for (auto& worker : workers_) {
    worker->Join();
  }
}

int WorkerGroup::Size() const {
  return workers_.size();
}

/**************
 * Worker
 ***************/

Worker::Worker(std::shared_ptr<TaskQueue> tq) : tq_(tq) {
}

void Worker::Start() {
  CHECK_EQ(0, uv_thread_create(&self_, _Run, reinterpret_cast<void *>(this)));
}

void Worker::Join() {
  CHECK_EQ(0, uv_thread_join(&self_));
}

void Worker::_Run(void* data) {
  Worker* worker = static_cast<Worker*>(data);

  TaskState::State task_state;
  while (std::unique_ptr<Task> task = worker->tq_->BlockingPop()) {
    // May have been cancelled while queued.
    task_state = task->TryUpdateState(TaskState::ASSIGNED);
    if (task_state == TaskState::ASSIGNED) {
      task->task_state_->MarkEnteredRun();
      task->Run();
      task->task_state_->MarkExitedRun();
    } else {
      CHECK_EQ(task_state, TaskState::CANCELLED);
      task->task_state_->MarkEnteredRun();
      task->task_state_->MarkExitedRun();
    }

    CHECK_EQ(task->TryUpdateState(TaskState::COMPLETED),
                                  TaskState::COMPLETED);
    worker->tq_->NotifyOfCompletion(std::move(task));
  }
}

/**************
 * Task
 ***************/

Task::Task() : task_state_() {
  details_.origin = TaskDetails::TASK_ORIGIN_UNKNOWN;
  details_.type = TaskDetails::TASK_TYPE_UNKNOWN;
  details_.size = TaskDetails::TASK_SIZE_UNKNOWN;
  details_.priority = -1;
  details_.cancelable = false;
}

void Task::SetTaskState(std::shared_ptr<TaskState> task_state) {
  task_state_ = task_state;
}

TaskState::State Task::TryUpdateState(TaskState::State new_state) {
  return task_state_->TryUpdateState(new_state);
}

/**************
 * TaskSummary
 ***************/

TaskSummary::TaskSummary(Task* completed_task) {
  details_ = completed_task->details_;
  time_in_queue_ = completed_task->task_state_->TimeInQueue();
  time_in_run_ = completed_task->task_state_->TimeInRun();
  time_at_completion_ = completed_task->task_state_->TimeAtCompletion();
}

/**************
 * TaskState
 ***************/

TaskState::TaskState() : lock_(), state_(INITIAL)
  , time_in_queue_(0), time_in_run_(0)
  , time_entered_queue_(0), time_exited_queue_(0)
  , time_entered_run_(0), time_exited_run_(0) {
}

TaskState::State TaskState::GetState() const {
  Mutex::ScopedLock scoped_lock(lock_);
  return state_;
}

bool TaskState::Cancel() {
  if (TryUpdateState(CANCELLED) == CANCELLED) {
    return true;
  }
  return false;
}

uint64_t TaskState::TimeInQueue() const {
  Mutex::ScopedLock scoped_lock(lock_);
  return time_in_queue_;
}

uint64_t TaskState::TimeInRun() const {
  Mutex::ScopedLock scoped_lock(lock_);
  return time_in_run_;
}

uint64_t TaskState::TimeAtCompletion() const {
  Mutex::ScopedLock scoped_lock(lock_);
  return time_exited_run_;
}

uint64_t TaskState::TimeInThreadpool() const {
  Mutex::ScopedLock scoped_lock(lock_);
  return time_in_queue_ + time_in_run_;
}



TaskState::State TaskState::TryUpdateState(TaskState::State new_state) {
  Mutex::ScopedLock scoped_lock(lock_);
  if (ValidStateTransition(state_, new_state)) {
    state_ = new_state;
  }
  return state_;
}

bool TaskState::
ValidStateTransition(TaskState::State old_state, TaskState::State new_state) {
  // Normal flow: INITIAL -> QUEUED -> ASSIGNED -> COMPLETED.
  // Also: non-terminal state -> CANCELLED -> COMPLETED.
  switch (old_state) {
    case INITIAL:
      return new_state == QUEUED || new_state == CANCELLED;
    case QUEUED:
      return new_state == ASSIGNED || new_state == CANCELLED;
    case ASSIGNED:
      return new_state == COMPLETED || new_state == CANCELLED;
    case CANCELLED:
      return new_state == COMPLETED;
    // No transitions out of terminal state.
    case COMPLETED:
      return false;
    default:
      CHECK(0);
  }
  return false;
}

void TaskState::MarkEnteredQueue() {
  Mutex::ScopedLock scoped_lock(lock_);
  time_entered_queue_ = uv_hrtime();
}

void TaskState::MarkExitedQueue() {
  Mutex::ScopedLock scoped_lock(lock_);
  time_exited_queue_ = uv_hrtime();
  CHECK_GE(time_exited_queue_, time_entered_queue_);
  time_in_queue_ = time_exited_queue_ - time_entered_queue_;
}

void TaskState::MarkEnteredRun() {
  Mutex::ScopedLock scoped_lock(lock_);
  time_entered_run_ = uv_hrtime();
}

void TaskState::MarkExitedRun() {
  Mutex::ScopedLock scoped_lock(lock_);
  time_exited_run_ = uv_hrtime();
  CHECK_GE(time_exited_run_, time_entered_run_);
  time_in_run_ = time_exited_run_ - time_entered_run_;
}

/**************
 * LibuvExecutor
 ***************/

class LibuvTaskData;
class LibuvTask;

// Internal LibuvExecutor mechanism to enable uv_cancel.
// Preserves task_state so smart pointers knows not to delete it.
class LibuvTaskData {
  friend class LibuvExecutor;

 public:
  explicit LibuvTaskData(std::shared_ptr<TaskState> state) : state_(state) {
  }

 private:
  std::shared_ptr<TaskState> state_;
};

// The LibuvExecutor wraps libuv uv_work_t's into LibuvTasks
// and routes them to the internal Threadpool.
class LibuvTask : public Task {
 public:
  LibuvTask(LibuvExecutor* libuv_executor,
            uv_work_t* req,
            const uv_work_options_t* opts)
    : Task(), libuv_executor_(libuv_executor), req_(req) {
    CHECK(req_);
    req_->reserved[0] = nullptr;

    details_.origin = TaskDetails::LIBUV;
    details_.size = TaskDetails::TASK_SIZE_UNKNOWN;

    // type
    if (opts) {
      switch (opts->type) {
        case UV_WORK_FS:
          details_.type = TaskDetails::FS;
          break;
        case UV_WORK_DNS:
          details_.type = TaskDetails::DNS;
          break;
        case UV_WORK_USER_IO:
          details_.type = TaskDetails::IO;
          break;
        case UV_WORK_USER_CPU:
          details_.type = TaskDetails::CPU;
          break;
        default:
          details_.type = TaskDetails::TASK_TYPE_UNKNOWN;
      }

      details_.priority = opts->priority;
      details_.cancelable = opts->cancelable;
    } else {
      details_.type = TaskDetails::TASK_TYPE_UNKNOWN;
      details_.priority = -1;
      details_.cancelable = false;
    }

    LOG("LibuvTask::LibuvTask: type %d\n", details_.type);
  }

  ~LibuvTask() {
    LOG("LibuvTask::Run: Task %p done\n", req_);
    // Clean up our storage.
    LibuvTaskData* data = reinterpret_cast<LibuvTaskData*>(req_->reserved[0]);
    delete data;
    req_->reserved[0] = nullptr;

    // Inform libuv.
    uv_executor_return_work(req_);
  }

  void Run() {
    LOG("LibuvTask::Run: Running Task %p\n", req_);
    req_->work_cb(req_);
  }

 private:
  LibuvExecutor* libuv_executor_;
  uv_work_t* req_;
};

LibuvExecutor::LibuvExecutor(std::shared_ptr<NodeThreadpool> tp)
  : tp_(tp) {
  executor_.submit = uv_executor_submit;
  executor_.cancel = uv_executor_cancel;
  executor_.data = this;
}

uv_executor_t* LibuvExecutor::GetExecutor() {
  return &executor_;
}

bool LibuvExecutor::Cancel(std::shared_ptr<TaskState> task_state) {
  return task_state->Cancel();
}

void LibuvExecutor::uv_executor_submit(uv_executor_t* executor,
                                       uv_work_t* req,
                                       const uv_work_options_t* opts) {
  LibuvExecutor* libuv_executor =
    reinterpret_cast<LibuvExecutor *>(executor->data);
  LOG("LibuvExecutor::uv_executor_submit: Got work %p\n", req);

  auto task_state = libuv_executor->tp_->Post(std::unique_ptr<Task>(
    new LibuvTask(libuv_executor, req, opts)));
  CHECK(task_state);  // Must not fail. We have no mechanism to tell libuv.

  auto data = new LibuvTaskData(task_state);
  req->reserved[0] = data;
}

// Remember, libuv user won't free uv_work_t until after its done_cb is called.
// That won't happen until after the wrapping LibuvTask is destroyed.
int LibuvExecutor::uv_executor_cancel(uv_executor_t* executor,
                                      uv_work_t* req) {
  if (!req || !req->reserved[0]) {
    return UV_EINVAL;
  }

  LibuvExecutor* libuv_executor =
    reinterpret_cast<LibuvExecutor *>(executor->data);
  LibuvTaskData* task_data =
    reinterpret_cast<LibuvTaskData *>(req->reserved[0]);

  if (libuv_executor->Cancel(task_data->state_)) {
    return 0;
  } else {
    return UV_EBUSY;
  }
}

/**************
 * TaskQueue
 ***************/

TaskQueue::TaskQueue(int id)
  : id_(id), lock_()
  , task_available_(), tasks_drained_()
  , queue_(), outstanding_tasks_(0), stopped_(false)
  , n_cpu_in_queue_(0), n_io_in_queue_(0)
  , n_changes_since_last_length_sample_(0), length_report_freq_(10)
  , task_summaries_(), queue_lengths_() {
}

bool TaskQueue::Push(std::unique_ptr<Task> task) {
  Mutex::ScopedLock scoped_lock(lock_);
  task->task_state_->MarkEnteredQueue();

  if (stopped_) {
    return false;
  }

  // The queue contains QUEUED or CANCELLED Tasks.
  // There's little harm in queueing CANCELLED tasks.
  TaskState::State task_state = task->TryUpdateState(TaskState::QUEUED);
  CHECK(task_state == TaskState::QUEUED || task_state == TaskState::CANCELLED);

  UpdateLength(task.get(), true);
  queue_.push(std::move(task));
  outstanding_tasks_++;
  task_available_.Signal(scoped_lock);

  return true;
}

void TaskQueue::UpdateLength(Task* task, bool grew) {
  int* counter = nullptr;
  if (task->details_.type == TaskDetails::CPU
    || task->details_.type == TaskDetails::MEMORY
    || task->details_.type == TaskDetails::TASK_TYPE_UNKNOWN) {
    counter = &n_cpu_in_queue_;
  } else {
    counter = &n_io_in_queue_;
  }

  if (grew) {
    (*counter)++;
  } else {
    (*counter)--;
  }
  CHECK_GE(*counter, 0);

  n_changes_since_last_length_sample_++;
  if (n_changes_since_last_length_sample_ == length_report_freq_) {
    queue_lengths_.push_back(
      std::unique_ptr<QueueLengthSample>(
        new QueueLengthSample(n_cpu_in_queue_, n_io_in_queue_, uv_hrtime())));
    n_changes_since_last_length_sample_ = 0;
  }
}

std::unique_ptr<Task> TaskQueue::Pop() {
  Mutex::ScopedLock scoped_lock(lock_);

  if (queue_.empty()) {
    return std::unique_ptr<Task>(nullptr);
  }

  std::unique_ptr<Task> task = std::move(queue_.front());
  task->task_state_->MarkExitedQueue();

  queue_.pop();
  UpdateLength(task.get(), false);
  return task;
}

std::unique_ptr<Task> TaskQueue::BlockingPop() {
  Mutex::ScopedLock scoped_lock(lock_);

  while (queue_.empty() && !stopped_) {
    task_available_.Wait(scoped_lock);
  }

  if (queue_.empty()) {
    return std::unique_ptr<Task>(nullptr);
  }

  std::unique_ptr<Task> task = std::move(queue_.front());
  task->task_state_->MarkExitedQueue();

  queue_.pop();
  UpdateLength(task.get(), false);
  return task;
}

void TaskQueue::NotifyOfCompletion(std::unique_ptr<Task> completed_task) {
  Mutex::ScopedLock scoped_lock(lock_);
  outstanding_tasks_--;
  CHECK_GE(outstanding_tasks_, 0);
  if (!outstanding_tasks_) {
    tasks_drained_.Broadcast(scoped_lock);
  }

  task_summaries_.push_back(
    std::unique_ptr<TaskSummary>(new TaskSummary(completed_task.get())));
}

void TaskQueue::BlockingDrain() {
  Mutex::ScopedLock scoped_lock(lock_);
  while (outstanding_tasks_) {
    tasks_drained_.Wait(scoped_lock);
  }
  LOG("TaskQueue::BlockingDrain: Fully drained\n");
}

void TaskQueue::Stop() {
  Mutex::ScopedLock scoped_lock(lock_);
  stopped_ = true;
  task_available_.Broadcast(scoped_lock);
}

int TaskQueue::Length() const {
  Mutex::ScopedLock scoped_lock(lock_);
  int length = n_cpu_in_queue_ + n_io_in_queue_;
  CHECK_EQ(queue_.size(), length);
  return length;
}

std::vector<std::unique_ptr<TaskSummary>> const&
TaskQueue::GetTaskSummaries() const {
  return task_summaries_;
}

std::vector<std::unique_ptr<QueueLengthSample>> const&
TaskQueue::GetQueueLengths() const {
  return queue_lengths_;
}

/**************
 * Threadpool
 ***************/

Threadpool::Threadpool(int threadpool_size, int id) : id_(id) {
  CHECK_GT(threadpool_size, 0);
  task_queue_ = std::make_shared<TaskQueue>(id);
  worker_group_ = std::unique_ptr<WorkerGroup>(
    new WorkerGroup(threadpool_size, task_queue_));
}

Threadpool::~Threadpool() {
  // Block future Push's.
  task_queue_->Stop();
  // As worker_group_ leaves scope, it drains tq and Join's its threads.
}

std::shared_ptr<TaskState> Threadpool::Post(std::unique_ptr<Task> task) {
  LOG("Threadpool::Post: Got task of type %d\n",
    task->details_.type);

  std::shared_ptr<TaskState> task_state = std::make_shared<TaskState>();
  task->SetTaskState(task_state);

  task_queue_->Push(std::move(task));

  return task_state;
}

int Threadpool::QueueLength() const {
  return task_queue_->Length();
}

void Threadpool::BlockingDrain() {
  task_queue_->BlockingDrain();
}

int Threadpool::NWorkers() const {
  return worker_group_->Size();
}

std::vector<std::unique_ptr<TaskSummary>> const&
Threadpool::GetTaskSummaries() const {
  return task_queue_->GetTaskSummaries();
}

std::vector<std::unique_ptr<QueueLengthSample>> const&
Threadpool::GetQueueLengths() const {
  return task_queue_->GetQueueLengths();
}

}  // namespace threadpool
}  // namespace node
