#include "node_buffer.h"
#include "node_internals.h"
#include "libplatform/libplatform.h"

#include <string>
#include "gtest/gtest.h"
#include "node_test_fixture.h"

using node::AtExit;
using node::RunAtExit;

static bool called_cb_1 = false;
static bool called_cb_2 = false;
static bool called_cb_ordered_1 = false;
static bool called_cb_ordered_2 = false;
static bool called_at_exit_js = false;
static void at_exit_callback1(void* arg);
static void at_exit_callback2(void* arg);
static void at_exit_callback_ordered1(void* arg);
static void at_exit_callback_ordered2(void* arg);
static void at_exit_js(void* arg);
static std::string cb_1_arg;  // NOLINT(runtime/string)

class EnvironmentTest : public EnvironmentTestFixture {
 private:
  void TearDown() override {
    NodeTestFixture::TearDown();
    called_cb_1 = false;
    called_cb_2 = false;
    called_cb_ordered_1 = false;
    called_cb_ordered_2 = false;
  }
};

TEST_F(EnvironmentTest, PreExecutionPreparation) {
  const v8::HandleScope handle_scope(isolate_);
  const Argv argv;
  Env env {handle_scope, argv};

  node::LoadEnvironment(*env, [&](const node::StartExecutionCallbackInfo& info)
                                  -> v8::MaybeLocal<v8::Value> {
    return v8::Null(isolate_);
  });

  v8::Local<v8::Context> context = isolate_->GetCurrentContext();
  const char* run_script = "process.argv0";
  v8::Local<v8::Script> script = v8::Script::Compile(
      context,
      v8::String::NewFromOneByte(isolate_,
                                 reinterpret_cast<const uint8_t*>(run_script),
                                 v8::NewStringType::kNormal).ToLocalChecked())
      .ToLocalChecked();
  v8::Local<v8::Value> result = script->Run(context).ToLocalChecked();
  CHECK(result->IsString());
}

TEST_F(EnvironmentTest, LoadEnvironmentWithCallback) {
  const v8::HandleScope handle_scope(isolate_);
  const Argv argv;
  Env env {handle_scope, argv};

  v8::Local<v8::Context> context = isolate_->GetCurrentContext();
  bool called_cb = false;
  node::LoadEnvironment(*env,
                        [&](const node::StartExecutionCallbackInfo& info)
                            -> v8::MaybeLocal<v8::Value> {
    called_cb = true;

    CHECK(info.process_object->IsObject());
    CHECK(info.native_require->IsFunction());

    v8::Local<v8::Value> argv0 = info.process_object->Get(
        context,
        v8::String::NewFromOneByte(
            isolate_,
            reinterpret_cast<const uint8_t*>("argv0"),
            v8::NewStringType::kNormal).ToLocalChecked()).ToLocalChecked();
    CHECK(argv0->IsString());

    return info.process_object;
  });

  CHECK(called_cb);
}

TEST_F(EnvironmentTest, LoadEnvironmentWithSource) {
  const v8::HandleScope handle_scope(isolate_);
  const Argv argv;
  Env env {handle_scope, argv};

  v8::Local<v8::Context> context = isolate_->GetCurrentContext();
  v8::Local<v8::Value> main_ret =
      node::LoadEnvironment(*env,
                            "return { process, require };").ToLocalChecked();

  CHECK(main_ret->IsObject());
  CHECK(main_ret.As<v8::Object>()->Get(
      context,
      v8::String::NewFromOneByte(
          isolate_,
          reinterpret_cast<const uint8_t*>("process"),
          v8::NewStringType::kNormal).ToLocalChecked())
          .ToLocalChecked()->IsObject());
  CHECK(main_ret.As<v8::Object>()->Get(
      context,
      v8::String::NewFromOneByte(
          isolate_,
          reinterpret_cast<const uint8_t*>("require"),
          v8::NewStringType::kNormal).ToLocalChecked())
          .ToLocalChecked()->IsFunction());
}

TEST_F(EnvironmentTest, AtExitWithEnvironment) {
  const v8::HandleScope handle_scope(isolate_);
  const Argv argv;
  Env env {handle_scope, argv};

  AtExit(*env, at_exit_callback1);
  RunAtExit(*env);
  EXPECT_TRUE(called_cb_1);
}

TEST_F(EnvironmentTest, AtExitWithoutEnvironment) {
  const v8::HandleScope handle_scope(isolate_);
  const Argv argv;
  Env env {handle_scope, argv};

  AtExit(at_exit_callback1);  // No Environment is passed to AtExit.
  RunAtExit(*env);
  EXPECT_TRUE(called_cb_1);
}

TEST_F(EnvironmentTest, AtExitOrder) {
  const v8::HandleScope handle_scope(isolate_);
  const Argv argv;
  Env env {handle_scope, argv};

  // Test that callbacks are run in reverse order.
  AtExit(*env, at_exit_callback_ordered1);
  AtExit(*env, at_exit_callback_ordered2);
  RunAtExit(*env);
  EXPECT_TRUE(called_cb_ordered_1);
  EXPECT_TRUE(called_cb_ordered_2);
}

TEST_F(EnvironmentTest, AtExitWithArgument) {
  const v8::HandleScope handle_scope(isolate_);
  const Argv argv;
  Env env {handle_scope, argv};

  std::string arg{"some args"};
  AtExit(*env, at_exit_callback1, static_cast<void*>(&arg));
  RunAtExit(*env);
  EXPECT_EQ(arg, cb_1_arg);
}

TEST_F(EnvironmentTest, AtExitRunsJS) {
  const v8::HandleScope handle_scope(isolate_);
  const Argv argv;
  Env env {handle_scope, argv};

  AtExit(*env, at_exit_js, static_cast<void*>(isolate_));
  EXPECT_FALSE(called_at_exit_js);
  RunAtExit(*env);
  EXPECT_TRUE(called_at_exit_js);
}

TEST_F(EnvironmentTest, MultipleEnvironmentsPerIsolate) {
  const v8::HandleScope handle_scope(isolate_);
  const Argv argv;
  // Only one of the Environments can have default flags and own the inspector.
  Env env1 {handle_scope, argv};
  Env env2 {handle_scope, argv, node::EnvironmentFlags::kNoFlags};

  AtExit(*env1, at_exit_callback1);
  AtExit(*env2, at_exit_callback2);
  RunAtExit(*env1);
  EXPECT_TRUE(called_cb_1);
  EXPECT_FALSE(called_cb_2);

  RunAtExit(*env2);
  EXPECT_TRUE(called_cb_2);
}

TEST_F(EnvironmentTest, NoEnvironmentSanity) {
  const v8::HandleScope handle_scope(isolate_);
  v8::Local<v8::Context> context = v8::Context::New(isolate_);
  EXPECT_EQ(node::Environment::GetCurrent(context), nullptr);
  EXPECT_EQ(node::GetCurrentEnvironment(context), nullptr);
  EXPECT_EQ(node::Environment::GetCurrent(isolate_), nullptr);

  v8::Context::Scope context_scope(context);
  EXPECT_EQ(node::Environment::GetCurrent(context), nullptr);
  EXPECT_EQ(node::GetCurrentEnvironment(context), nullptr);
  EXPECT_EQ(node::Environment::GetCurrent(isolate_), nullptr);
}

TEST_F(EnvironmentTest, NonNodeJSContext) {
  const v8::HandleScope handle_scope(isolate_);
  const Argv argv;
  Env test_env {handle_scope, argv};

  EXPECT_EQ(node::Environment::GetCurrent(v8::Local<v8::Context>()), nullptr);

  node::Environment* env = *test_env;
  EXPECT_EQ(node::Environment::GetCurrent(isolate_), env);
  EXPECT_EQ(node::Environment::GetCurrent(env->context()), env);
  EXPECT_EQ(node::GetCurrentEnvironment(env->context()), env);

  v8::Local<v8::Context> context = v8::Context::New(isolate_);
  EXPECT_EQ(node::Environment::GetCurrent(context), nullptr);
  EXPECT_EQ(node::GetCurrentEnvironment(context), nullptr);
  EXPECT_EQ(node::Environment::GetCurrent(isolate_), env);

  v8::Context::Scope context_scope(context);
  EXPECT_EQ(node::Environment::GetCurrent(context), nullptr);
  EXPECT_EQ(node::GetCurrentEnvironment(context), nullptr);
  EXPECT_EQ(node::Environment::GetCurrent(isolate_), nullptr);
}

static void at_exit_callback1(void* arg) {
  called_cb_1 = true;
  if (arg) {
    cb_1_arg = *static_cast<std::string*>(arg);
  }
}

static void at_exit_callback2(void* arg) {
  called_cb_2 = true;
}

static void at_exit_callback_ordered1(void* arg) {
  EXPECT_TRUE(called_cb_ordered_2);
  called_cb_ordered_1 = true;
}

static void at_exit_callback_ordered2(void* arg) {
  EXPECT_FALSE(called_cb_ordered_1);
  called_cb_ordered_2 = true;
}

static void at_exit_js(void* arg) {
  v8::Isolate* isolate = static_cast<v8::Isolate*>(arg);
  v8::HandleScope handle_scope(isolate);
  v8::Local<v8::Object> obj = v8::Object::New(isolate);
  assert(!obj.IsEmpty());  // Assert VM is still alive.
  assert(obj->IsObject());
  called_at_exit_js = true;
}

TEST_F(EnvironmentTest, SetImmediateCleanup) {
  int called = 0;
  int called_unref = 0;

  {
    const v8::HandleScope handle_scope(isolate_);
    const Argv argv;
    Env env {handle_scope, argv};

    node::LoadEnvironment(*env,
                          [&](const node::StartExecutionCallbackInfo& info)
                              -> v8::MaybeLocal<v8::Value> {
      return v8::Object::New(isolate_);
    });

    (*env)->SetImmediate([&](node::Environment* env_arg) {
      EXPECT_EQ(env_arg, *env);
      called++;
    });
    (*env)->SetUnrefImmediate([&](node::Environment* env_arg) {
      EXPECT_EQ(env_arg, *env);
      called_unref++;
    });
  }

  EXPECT_EQ(called, 1);
  EXPECT_EQ(called_unref, 0);
}

static char hello[] = "hello";

TEST_F(EnvironmentTest, BufferWithFreeCallbackIsDetached) {
  // Test that a Buffer allocated with a free callback is detached after
  // its callback has been called.
  const v8::HandleScope handle_scope(isolate_);
  const Argv argv;

  int callback_calls = 0;

  v8::Local<v8::ArrayBuffer> ab;
  {
    Env env {handle_scope, argv};
    v8::Local<v8::Object> buf_obj = node::Buffer::New(
        isolate_,
        hello,
        sizeof(hello),
        [](char* data, void* hint) {
          CHECK_EQ(data, hello);
          ++*static_cast<int*>(hint);
        },
        &callback_calls).ToLocalChecked();
    CHECK(buf_obj->IsUint8Array());
    ab = buf_obj.As<v8::Uint8Array>()->Buffer();
    CHECK_EQ(ab->ByteLength(), sizeof(hello));
  }

  CHECK_EQ(callback_calls, 1);
  CHECK_EQ(ab->ByteLength(), 0);
}

#if HAVE_INSPECTOR
TEST_F(EnvironmentTest, InspectorMultipleEmbeddedEnvironments) {
  // Tests that child Environments can be created through the public API
  // that are accessible by the inspector.
  // This test sets a global variable in the child Environment, and reads it
  // back both through the inspector and inside the child Environment, and
  // makes sure that those correspond to the value that was originally set.
  const v8::HandleScope handle_scope(isolate_);
  const Argv argv;
  Env env {handle_scope, argv};

  v8::Local<v8::Context> context = isolate_->GetCurrentContext();
  node::LoadEnvironment(*env,
      "'use strict';\n"
      "const { Worker } = require('worker_threads');\n"
      "const { Session } = require('inspector');\n"

      "const session = new Session();\n"
      "session.connect();\n"
      "session.on('NodeWorker.attachedToWorker', (\n"
      "  ({ params: { workerInfo, sessionId } }) => {\n"
      "    session.post('NodeWorker.sendMessageToWorker', {\n"
      "      sessionId,\n"
      "      message: JSON.stringify({\n"
      "        id: 1,\n"
      "        method: 'Runtime.evaluate',\n"
      "        params: {\n"
      "          expression: 'globalThis.variableFromParent = 42;'\n"
      "        }\n"
      "      })\n"
      "    });\n"
      "    session.on('NodeWorker.receivedMessageFromWorker',\n"
      "      ({ params: { message } }) => {\n"
      "        global.messageFromWorker = \n"
      "          JSON.parse(message).result.result.value;\n"
      "      });\n"
      "  }));\n"
      "session.post('NodeWorker.enable', { waitForDebuggerOnStart: false });\n")
          .ToLocalChecked();

  struct ChildEnvironmentData {
    node::ThreadId thread_id;
    std::unique_ptr<node::InspectorParentHandle> inspector_parent_handle;
    node::MultiIsolatePlatform* platform;
    int32_t extracted_value = -1;
    uv_async_t thread_stopped_async;
  };

  ChildEnvironmentData data;
  data.thread_id = node::AllocateEnvironmentThreadId();
  data.inspector_parent_handle =
      GetInspectorParentHandle(*env, data.thread_id, "file:///embedded.js");
  CHECK(data.inspector_parent_handle);
  data.platform = GetMultiIsolatePlatform(*env);
  CHECK_NOT_NULL(data.platform);

  bool thread_stopped = false;
  int err = uv_async_init(
      &current_loop, &data.thread_stopped_async, [](uv_async_t* async) {
        *static_cast<bool*>(async->data) = true;
        uv_close(reinterpret_cast<uv_handle_t*>(async), nullptr);
      });
  CHECK_EQ(err, 0);
  data.thread_stopped_async.data = &thread_stopped;

  uv_thread_t thread;
  err = uv_thread_create(&thread, [](void* arg) {
    ChildEnvironmentData* data = static_cast<ChildEnvironmentData*>(arg);
    std::shared_ptr<node::ArrayBufferAllocator> aba =
        node::ArrayBufferAllocator::Create();
    uv_loop_t loop;
    uv_loop_init(&loop);
    v8::Isolate* isolate = NewIsolate(aba, &loop, data->platform);
    CHECK_NOT_NULL(isolate);

    {
      v8::Isolate::Scope isolate_scope(isolate);
      v8::HandleScope handle_scope(isolate);

      v8::Local<v8::Context> context = node::NewContext(isolate);
      CHECK(!context.IsEmpty());
      v8::Context::Scope context_scope(context);

      node::IsolateData* isolate_data = node::CreateIsolateData(
          isolate,
          &loop,
          data->platform);
      CHECK_NOT_NULL(isolate_data);
      node::Environment* environment = node::CreateEnvironment(
          isolate_data,
          context,
          { "dummy" },
          {},
          node::EnvironmentFlags::kNoFlags,
          data->thread_id,
          std::move(data->inspector_parent_handle));
      CHECK_NOT_NULL(environment);

      v8::Local<v8::Value> extracted_value = LoadEnvironment(
          environment,
          "while (!global.variableFromParent) {}\n"
          "return global.variableFromParent;").ToLocalChecked();

      uv_run(&loop, UV_RUN_DEFAULT);
      CHECK(extracted_value->IsInt32());
      data->extracted_value = extracted_value.As<v8::Int32>()->Value();

      node::FreeEnvironment(environment);
      node::FreeIsolateData(isolate_data);
    }

    data->platform->UnregisterIsolate(isolate);
    isolate->Dispose();
    uv_run(&loop, UV_RUN_DEFAULT);
    CHECK_EQ(uv_loop_close(&loop), 0);

    uv_async_send(&data->thread_stopped_async);
  }, &data);
  CHECK_EQ(err, 0);

  bool more;
  do {
    uv_run(&current_loop, UV_RUN_DEFAULT);
    data.platform->DrainTasks(isolate_);
    more = uv_loop_alive(&current_loop);
  } while (!thread_stopped || more);

  uv_thread_join(&thread);

  v8::Local<v8::Value> from_inspector =
      context->Global()->Get(
          context,
          v8::String::NewFromOneByte(
              isolate_,
              reinterpret_cast<const uint8_t*>("messageFromWorker"),
              v8::NewStringType::kNormal).ToLocalChecked())
              .ToLocalChecked();
  CHECK_EQ(data.extracted_value, 42);
  CHECK_EQ(from_inspector->IntegerValue(context).FromJust(), 42);
}
#endif  // HAVE_INSPECTOR

TEST_F(EnvironmentTest, ExitHandlerTest) {
  const v8::HandleScope handle_scope(isolate_);
  const Argv argv;

  int callback_calls = 0;

  Env env {handle_scope, argv};
  SetProcessExitHandler(*env, [&](node::Environment* env_, int exit_code) {
    EXPECT_EQ(*env, env_);
    EXPECT_EQ(exit_code, 42);
    callback_calls++;
    node::Stop(*env);
  });
  node::LoadEnvironment(*env, "process.exit(42)").ToLocalChecked();
  EXPECT_EQ(callback_calls, 1);
}
