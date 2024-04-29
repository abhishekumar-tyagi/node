// Copyright 2017 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/libplatform/default-foreground-task-runner.h"

#include "src/base/platform/mutex.h"
#include "src/base/platform/time.h"

namespace v8 {
namespace platform {

DefaultForegroundTaskRunner::RunTaskScope::RunTaskScope(
    std::shared_ptr<DefaultForegroundTaskRunner> task_runner)
    : task_runner_(task_runner) {
  DCHECK_GE(task_runner->nesting_depth_, 0);
  task_runner->nesting_depth_++;
}

DefaultForegroundTaskRunner::RunTaskScope::~RunTaskScope() {
  DCHECK_GT(task_runner_->nesting_depth_, 0);
  task_runner_->nesting_depth_--;
}

DefaultForegroundTaskRunner::DefaultForegroundTaskRunner(
    IdleTaskSupport idle_task_support, TimeFunction time_function)
    : idle_task_support_(idle_task_support), time_function_(time_function) {}

void DefaultForegroundTaskRunner::Terminate() {
  // Drain the task queues.
  // We make sure to delete tasks outside the TaskRunner lock, to avoid
  // potential deadlocks.
  std::deque<TaskQueueEntry> obsolete_tasks;
  std::priority_queue<DelayedEntry, std::vector<DelayedEntry>,
                      DelayedEntryCompare>
      obsolete_delayed_tasks;
  std::queue<std::unique_ptr<IdleTask>> obsolete_idle_tasks;
  {
    base::MutexGuard guard(&lock_);
    terminated_ = true;
    task_queue_.swap(obsolete_tasks);
    delayed_task_queue_.swap(obsolete_delayed_tasks);
    idle_task_queue_.swap(obsolete_idle_tasks);
  }
  while (!obsolete_tasks.empty()) obsolete_tasks.pop_front();
  while (!obsolete_delayed_tasks.empty()) obsolete_delayed_tasks.pop();
  while (!obsolete_idle_tasks.empty()) obsolete_idle_tasks.pop();
}

void DefaultForegroundTaskRunner::PostTaskLocked(std::unique_ptr<Task> task,
                                                 Nestability nestability,
                                                 const base::MutexGuard&) {
  if (terminated_) return;
  task_queue_.push_back(std::make_pair(nestability, std::move(task)));
  event_loop_control_.NotifyOne();
}

void DefaultForegroundTaskRunner::PostTaskImpl(std::unique_ptr<Task> task,
                                               const SourceLocation& location) {
  base::MutexGuard guard(&lock_);
  PostTaskLocked(std::move(task), kNestable, guard);
}

double DefaultForegroundTaskRunner::MonotonicallyIncreasingTime() {
  return time_function_();
}

void DefaultForegroundTaskRunner::PostDelayedTaskLocked(
    std::unique_ptr<Task> task, double delay_in_seconds,
    Nestability nestability, const base::MutexGuard&) {
  DCHECK_GE(delay_in_seconds, 0.0);
  if (terminated_) return;
  double deadline = MonotonicallyIncreasingTime() + delay_in_seconds;
  delayed_task_queue_.push({deadline, nestability, std::move(task)});
  event_loop_control_.NotifyOne();
}

void DefaultForegroundTaskRunner::PostDelayedTaskImpl(
    std::unique_ptr<Task> task, double delay_in_seconds,
    const SourceLocation& location) {
  base::MutexGuard guard(&lock_);
  PostDelayedTaskLocked(std::move(task), delay_in_seconds, kNestable, guard);
}

void DefaultForegroundTaskRunner::PostNonNestableDelayedTaskImpl(
    std::unique_ptr<Task> task, double delay_in_seconds,
    const SourceLocation& location) {
  base::MutexGuard guard(&lock_);
  PostDelayedTaskLocked(std::move(task), delay_in_seconds, kNonNestable, guard);
}

void DefaultForegroundTaskRunner::PostIdleTaskImpl(
    std::unique_ptr<IdleTask> task, const SourceLocation& location) {
  CHECK_EQ(IdleTaskSupport::kEnabled, idle_task_support_);
  base::MutexGuard guard(&lock_);
  if (terminated_) return;
  idle_task_queue_.push(std::move(task));
}

bool DefaultForegroundTaskRunner::IdleTasksEnabled() {
  return idle_task_support_ == IdleTaskSupport::kEnabled;
}

void DefaultForegroundTaskRunner::PostNonNestableTaskImpl(
    std::unique_ptr<Task> task, const SourceLocation& location) {
  base::MutexGuard guard(&lock_);
  PostTaskLocked(std::move(task), kNonNestable, guard);
}

bool DefaultForegroundTaskRunner::NonNestableTasksEnabled() const {
  return true;
}

bool DefaultForegroundTaskRunner::HasPoppableTaskInQueue() const {
  if (nesting_depth_ == 0) return !task_queue_.empty();
  for (auto it = task_queue_.cbegin(); it != task_queue_.cend(); it++) {
    if (it->first == kNestable) return true;
  }
  return false;
}

void DefaultForegroundTaskRunner::MoveExpiredDelayedTasks(
    const base::MutexGuard& guard) {
  Nestability nestability;
  std::unique_ptr<Task> task =
      PopTaskFromDelayedQueueLocked(guard, &nestability);
  while (task) {
    PostTaskLocked(std::move(task), nestability, guard);
    task = PopTaskFromDelayedQueueLocked(guard, &nestability);
  }
}

std::unique_ptr<Task> DefaultForegroundTaskRunner::PopTaskFromQueue(
    MessageLoopBehavior wait_for_work) {
  base::MutexGuard guard(&lock_);
  MoveExpiredDelayedTasks(guard);

  while (!HasPoppableTaskInQueue()) {
    if (wait_for_work == MessageLoopBehavior::kDoNotWait) return {};
    WaitForTaskLocked(guard);
    MoveExpiredDelayedTasks(guard);
  }

  auto it = task_queue_.begin();
  for (; it != task_queue_.end(); it++) {
    // When the task queue is nested (i.e. popping a task from the queue from
    // within a task), only nestable tasks may run. Otherwise, any task may run.
    if (nesting_depth_ == 0 || it->first == kNestable) break;
  }
  DCHECK(it != task_queue_.end());
  std::unique_ptr<Task> task = std::move(it->second);
  task_queue_.erase(it);

  return task;
}

std::unique_ptr<Task>
DefaultForegroundTaskRunner::PopTaskFromDelayedQueueLocked(
    const base::MutexGuard&, Nestability* nestability) {
  if (delayed_task_queue_.empty()) return {};

  double now = MonotonicallyIncreasingTime();
  const DelayedEntry& entry = delayed_task_queue_.top();
  if (entry.timeout_time > now) return {};
  // The const_cast here is necessary because there does not exist a clean way
  // to get a unique_ptr out of the priority queue. We provide the priority
  // queue with a custom comparison operator to make sure that the priority
  // queue does not access the unique_ptr. Therefore it should be safe to reset
  // the unique_ptr in the priority queue here. Note that the DelayedEntry is
  // removed from the priority_queue immediately afterwards.
  std::unique_ptr<Task> task = std::move(const_cast<DelayedEntry&>(entry).task);
  *nestability = entry.nestability;
  delayed_task_queue_.pop();
  return task;
}

std::unique_ptr<IdleTask> DefaultForegroundTaskRunner::PopTaskFromIdleQueue() {
  base::MutexGuard guard(&lock_);
  if (idle_task_queue_.empty()) return {};

  std::unique_ptr<IdleTask> task = std::move(idle_task_queue_.front());
  idle_task_queue_.pop();

  return task;
}

void DefaultForegroundTaskRunner::WaitForTaskLocked(const base::MutexGuard&) {
  if (!delayed_task_queue_.empty()) {
    double now = MonotonicallyIncreasingTime();
    const DelayedEntry& entry = delayed_task_queue_.top();
    double time_until_task = entry.timeout_time - now;
    if (time_until_task > 0) {
      bool woken_up = event_loop_control_.WaitFor(
          &lock_,
          base::TimeDelta::FromMicroseconds(
              time_until_task * base::TimeConstants::kMicrosecondsPerSecond));
      USE(woken_up);
    }
  } else {
    event_loop_control_.Wait(&lock_);
  }
}

}  // namespace platform
}  // namespace v8
