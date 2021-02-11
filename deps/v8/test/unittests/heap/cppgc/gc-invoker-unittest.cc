// Copyright 2020 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/heap/cppgc/gc-invoker.h"

#include "include/cppgc/platform.h"
#include "src/heap/cppgc/heap.h"
#include "test/unittests/heap/cppgc/test-platform.h"
#include "testing/gmock/include/gmock/gmock-matchers.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace cppgc {
namespace internal {

namespace {

class MockGarbageCollector : public GarbageCollector {
 public:
  MOCK_METHOD(void, CollectGarbage, (GarbageCollector::Config), (override));
  MOCK_METHOD(void, StartIncrementalGarbageCollection,
              (GarbageCollector::Config), (override));
  MOCK_METHOD(size_t, epoch, (), (const, override));
};

class MockTaskRunner : public cppgc::TaskRunner {
 public:
  MOCK_METHOD(void, PostTask, (std::unique_ptr<cppgc::Task>), (override));
  MOCK_METHOD(void, PostNonNestableTask, (std::unique_ptr<cppgc::Task>),
              (override));
  MOCK_METHOD(void, PostDelayedTask, (std::unique_ptr<cppgc::Task>, double),
              (override));
  MOCK_METHOD(void, PostNonNestableDelayedTask,
              (std::unique_ptr<cppgc::Task>, double), (override));
  MOCK_METHOD(void, PostIdleTask, (std::unique_ptr<cppgc::IdleTask>),
              (override));

  virtual bool IdleTasksEnabled() override { return true; }       // NOLINT
  bool NonNestableTasksEnabled() const override { return true; }  // NOLINT
  virtual bool NonNestableDelayedTasksEnabled() const override {  // NOLINT
    return true;
  }
};

class MockPlatform : public cppgc::Platform {
 public:
  explicit MockPlatform(std::shared_ptr<TaskRunner> runner)
      : runner_(std::move(runner)) {}

  PageAllocator* GetPageAllocator() override { return nullptr; }
  double MonotonicallyIncreasingTime() override { return 0.0; }

  std::shared_ptr<TaskRunner> GetForegroundTaskRunner() override {
    return runner_;
  }

 private:
  std::shared_ptr<TaskRunner> runner_;
};

}  // namespace

TEST(GCInvokerTest, PrecideGCIsInvokedSynchronously) {
  MockPlatform platform(nullptr);
  MockGarbageCollector gc;
  GCInvoker invoker(&gc, &platform,
                    cppgc::Heap::StackSupport::kNoConservativeStackScan);
  EXPECT_CALL(gc, CollectGarbage(::testing::Field(
                      &GarbageCollector::Config::stack_state,
                      GarbageCollector::Config::StackState::kNoHeapPointers)));
  invoker.CollectGarbage(GarbageCollector::Config::PreciseAtomicConfig());
}

TEST(GCInvokerTest, ConservativeGCIsInvokedSynchronouslyWhenSupported) {
  MockPlatform platform(nullptr);
  MockGarbageCollector gc;
  GCInvoker invoker(&gc, &platform,
                    cppgc::Heap::StackSupport::kSupportsConservativeStackScan);
  EXPECT_CALL(
      gc, CollectGarbage(::testing::Field(
              &GarbageCollector::Config::stack_state,
              GarbageCollector::Config::StackState::kMayContainHeapPointers)));
  invoker.CollectGarbage(GarbageCollector::Config::ConservativeAtomicConfig());
}

TEST(GCInvokerTest, ConservativeGCIsScheduledAsPreciseGCViaPlatform) {
  std::shared_ptr<cppgc::TaskRunner> runner =
      std::shared_ptr<cppgc::TaskRunner>(new MockTaskRunner());
  MockPlatform platform(runner);
  MockGarbageCollector gc;
  GCInvoker invoker(&gc, &platform,
                    cppgc::Heap::StackSupport::kNoConservativeStackScan);
  EXPECT_CALL(gc, epoch).WillOnce(::testing::Return(0));
  EXPECT_CALL(*static_cast<MockTaskRunner*>(runner.get()),
              PostNonNestableTask(::testing::_));
  invoker.CollectGarbage(GarbageCollector::Config::ConservativeAtomicConfig());
}

TEST(GCInvokerTest, ConservativeGCIsInvokedAsPreciseGCViaPlatform) {
  testing::TestPlatform platform;
  MockGarbageCollector gc;
  GCInvoker invoker(&gc, &platform,
                    cppgc::Heap::StackSupport::kNoConservativeStackScan);
  EXPECT_CALL(gc, epoch).WillRepeatedly(::testing::Return(0));
  EXPECT_CALL(gc, CollectGarbage);
  invoker.CollectGarbage(GarbageCollector::Config::ConservativeAtomicConfig());
  platform.RunAllForegroundTasks();
}

TEST(GCInvokerTest, IncrementalGCIsStarted) {
  // Since StartIncrementalGarbageCollection doesn't scan the stack, support for
  // conservative stack scanning should not matter.
  MockPlatform platform(nullptr);
  MockGarbageCollector gc;
  // Conservative stack scanning supported.
  GCInvoker invoker_with_support(
      &gc, &platform,
      cppgc::Heap::StackSupport::kSupportsConservativeStackScan);
  EXPECT_CALL(
      gc, StartIncrementalGarbageCollection(::testing::Field(
              &GarbageCollector::Config::stack_state,
              GarbageCollector::Config::StackState::kMayContainHeapPointers)));
  invoker_with_support.StartIncrementalGarbageCollection(
      GarbageCollector::Config::ConservativeIncrementalConfig());
  // Conservative stack scanning *not* supported.
  GCInvoker invoker_without_support(
      &gc, &platform, cppgc::Heap::StackSupport::kNoConservativeStackScan);
  EXPECT_CALL(
      gc, StartIncrementalGarbageCollection(::testing::Field(
              &GarbageCollector::Config::stack_state,
              GarbageCollector::Config::StackState::kMayContainHeapPointers)))
      .Times(0);
  invoker_without_support.StartIncrementalGarbageCollection(
      GarbageCollector::Config::ConservativeIncrementalConfig());
}

}  // namespace internal
}  // namespace cppgc
