#include "main_thread_interface.h"

#include "node_mutex.h"
#include "v8-inspector.h"

#include <unicode/unistr.h>

namespace node {
namespace inspector {
namespace {

using v8_inspector::StringBuffer;
using v8_inspector::StringView;

template <typename T>
class DeleteRequest : public Request {
 public:
  explicit DeleteRequest(T* object) : object_(object) {}
  void Call() override { delete object_; }

 private:
  T* object_;
};

template <typename Target, typename Arg>
class SingleArgumentFunctionCall : public Request {
 public:
  using Fn = void (Target::*)(Arg);

  SingleArgumentFunctionCall(Target* target, Fn fn, Arg argument)
      : target_(target), fn_(fn), arg_(std::move(argument)) {}

  void Call() override { Apply(target_, fn_, std::move(arg_)); }

 private:
  template <typename Element>
  void Apply(Element* target, Fn fn, Arg arg) {
    (target->*fn)(std::move(arg));
  }

  Target* target_;
  Fn fn_;
  Arg arg_;
};

class PostMessageRequest : public Request {
 public:
  PostMessageRequest(InspectorSessionDelegate* delegate, StringView message)
      : delegate_(delegate), message_(StringBuffer::create(message)) {}

  void Call() override { delegate_->SendMessageToFrontend(message_->string()); }

 private:
  InspectorSessionDelegate* delegate_;
  std::unique_ptr<StringBuffer> message_;
};

class DispatchMessagesTask : public v8::Task {
 public:
  explicit DispatchMessagesTask(MainThreadInterface* thread)
      : thread_(thread) {}

  void Run() override { thread_->DispatchMessages(); }

 private:
  MainThreadInterface* thread_;
};

void DisposePairCallback(uv_handle_t* ref) {
  using AsyncAndInterface = std::pair<uv_async_t, MainThreadInterface*>;
  AsyncAndInterface* pair = node::ContainerOf(
      &AsyncAndInterface::first, reinterpret_cast<uv_async_t*>(ref));
  delete pair;
}

template <typename T>
class AnotherThreadObjectReference {
 public:
  // We create it on whatever thread, just make sure it gets disposed on the
  // proper thread.
  AnotherThreadObjectReference(std::shared_ptr<MainThreadHandle> thread,
                               T* object)
      : thread_(thread), object_(object) {}
  AnotherThreadObjectReference(AnotherThreadObjectReference&) = delete;

  ~AnotherThreadObjectReference() {
    // Disappearing thread may cause a memory leak
    CHECK(thread_->Post(
        std::unique_ptr<DeleteRequest<T>>(new DeleteRequest<T>(object_))));
    object_ = nullptr;
  }

  template <typename Fn, typename Arg>
  void Post(Fn fn, Arg argument) const {
    using R = SingleArgumentFunctionCall<T, Arg>;
    thread_->Post(std::unique_ptr<R>(new R(object_, fn, std::move(argument))));
  }

  T* get() const { return object_; }

 private:
  std::shared_ptr<MainThreadHandle> thread_;
  T* object_;
};

class MainThreadSessionState {
 public:
  MainThreadSessionState(std::shared_ptr<MainThreadHandle> thread,
                         bool prevent_shutdown)
      : thread_(thread), prevent_shutdown_(prevent_shutdown) {}

  void Connect(std::unique_ptr<InspectorSessionDelegate> delegate) {
    Agent* agent = thread_->GetInspectorAgent();
    if (agent != nullptr)
      session_ = agent->Connect(std::move(delegate), prevent_shutdown_);
  }

  void Dispatch(std::unique_ptr<StringBuffer> message) {
    session_->Dispatch(message->string());
  }

 private:
  std::shared_ptr<MainThreadHandle> thread_;
  bool prevent_shutdown_;
  std::unique_ptr<InspectorSession> session_;
};

class CrossThreadInspectorSession : public InspectorSession {
 public:
  CrossThreadInspectorSession(
      int id,
      std::shared_ptr<MainThreadHandle> thread,
      std::unique_ptr<InspectorSessionDelegate> delegate,
      bool prevent_shutdown)
      : state_(thread, new MainThreadSessionState(thread, prevent_shutdown)) {
    state_.Post(&MainThreadSessionState::Connect, std::move(delegate));
  }

  void Dispatch(const StringView& message) override {
    state_.Post(&MainThreadSessionState::Dispatch,
                StringBuffer::create(message));
  }

 private:
  AnotherThreadObjectReference<MainThreadSessionState> state_;
};

class ThreadSafeDelegate : public InspectorSessionDelegate {
 public:
  ThreadSafeDelegate(std::shared_ptr<MainThreadHandle> thread,
                     std::unique_ptr<InspectorSessionDelegate> delegate)
      : thread_(thread), delegate_(thread, delegate.release()) {}

  void SendMessageToFrontend(const v8_inspector::StringView& message) override {
    thread_->Post(std::unique_ptr<Request>(
        new PostMessageRequest(delegate_.get(), message)));
  }

 private:
  std::shared_ptr<MainThreadHandle> thread_;
  AnotherThreadObjectReference<InspectorSessionDelegate> delegate_;
};
}  // namespace

MainThreadInterface::MainThreadInterface(Agent* agent,
                                         uv_loop_t* loop,
                                         v8::Isolate* isolate,
                                         v8::Platform* platform)
    : agent_(agent), isolate_(isolate), platform_(platform) {
  main_thread_request_.reset(new AsyncAndInterface(uv_async_t(), this));
  CHECK_EQ(
      0,
      uv_async_init(
          loop, &main_thread_request_->first, DispatchMessagesAsyncCallback));
  // Inspector uv_async_t should not prevent main loop shutdown.
  uv_unref(reinterpret_cast<uv_handle_t*>(&main_thread_request_->first));
}

MainThreadInterface::~MainThreadInterface() {
  if (handle_) handle_->Reset();
}

// static
void MainThreadInterface::DispatchMessagesAsyncCallback(uv_async_t* async) {
  AsyncAndInterface* asyncAndInterface =
      node::ContainerOf(&AsyncAndInterface::first, async);
  asyncAndInterface->second->DispatchMessages();
}

// static
void MainThreadInterface::CloseAsync(AsyncAndInterface* pair) {
  uv_close(reinterpret_cast<uv_handle_t*>(&pair->first), DisposePairCallback);
}

void MainThreadInterface::Post(std::unique_ptr<Request> request) {
  Mutex::ScopedLock scoped_lock(requests_lock_);
  bool needs_notify = requests_.empty();
  requests_.push_back(std::move(request));
  if (needs_notify) {
    CHECK_EQ(0, uv_async_send(&main_thread_request_->first));
    if (isolate_ != nullptr && platform_ != nullptr) {
      platform_->CallOnForegroundThread(isolate_,
                                        new DispatchMessagesTask(this));
      isolate_->RequestInterrupt(
          [](v8::Isolate* isolate, void* thread) {
            static_cast<MainThreadInterface*>(thread)->DispatchMessages();
          },
          this);
    }
  }
  incoming_message_cond_.Broadcast(scoped_lock);
}

bool MainThreadInterface::WaitForFrontendEvent() {
  // We allow DispatchMessages reentry as we enter the pause. This is important
  // to support debugging the code invoked by an inspector call, such
  // as Runtime.evaluate
  dispatching_messages_ = false;
  if (dispatching_message_queue_.empty()) {
    Mutex::ScopedLock scoped_lock(requests_lock_);
    while (requests_.empty()) incoming_message_cond_.Wait(scoped_lock);
  }
  return true;
}

void MainThreadInterface::DispatchMessages() {
  if (dispatching_messages_) return;
  dispatching_messages_ = true;
  bool had_messages = false;
  do {
    if (dispatching_message_queue_.empty()) {
      Mutex::ScopedLock scoped_lock(requests_lock_);
      requests_.swap(dispatching_message_queue_);
    }
    had_messages = !dispatching_message_queue_.empty();
    while (!dispatching_message_queue_.empty()) {
      MessageQueue::value_type task;
      std::swap(dispatching_message_queue_.front(), task);
      dispatching_message_queue_.pop_front();
      task->Call();
    }
  } while (had_messages);
  dispatching_messages_ = false;
}

std::shared_ptr<MainThreadHandle> MainThreadInterface::GetHandle() {
  if (handle_ == nullptr) handle_ = std::make_shared<MainThreadHandle>(this);
  return handle_;
}

std::unique_ptr<StringBuffer> Utf8ToStringView(const std::string& message) {
  icu::UnicodeString utf16 = icu::UnicodeString::fromUTF8(
      icu::StringPiece(message.data(), message.length()));
  StringView view(reinterpret_cast<const uint16_t*>(utf16.getBuffer()),
                  utf16.length());
  return StringBuffer::create(view);
}

std::unique_ptr<InspectorSession> MainThreadHandle::Connect(
    std::unique_ptr<InspectorSessionDelegate> delegate, bool prevent_shutdown) {
  return std::unique_ptr<InspectorSession>(
      new CrossThreadInspectorSession(++next_session_id_,
                                      shared_from_this(),
                                      std::move(delegate),
                                      prevent_shutdown));
}

bool MainThreadHandle::Post(std::unique_ptr<Request> request) {
  Mutex::ScopedLock scoped_lock(block_lock_);
  if (!main_thread_) return false;
  main_thread_->Post(std::move(request));
  return true;
}

void MainThreadHandle::Reset() {
  Mutex::ScopedLock scoped_lock(block_lock_);
  main_thread_ = nullptr;
}

Agent* MainThreadHandle::GetInspectorAgent() {
  Mutex::ScopedLock scoped_lock(block_lock_);
  if (main_thread_ == nullptr) return nullptr;
  return main_thread_->inspector_agent();
}

std::unique_ptr<InspectorSessionDelegate>
MainThreadHandle::MakeThreadSafeDelegate(
    std::unique_ptr<InspectorSessionDelegate> delegate) {
  return std::unique_ptr<InspectorSessionDelegate>(
      new ThreadSafeDelegate(shared_from_this(), std::move(delegate)));
}

bool MainThreadHandle::Expired() {
  Mutex::ScopedLock scoped_lock(block_lock_);
  return main_thread_ == nullptr;
}
}  // namespace inspector
}  // namespace node
