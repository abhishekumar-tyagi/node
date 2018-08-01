#include <node.h>
#include <uv.h>
#include <v8.h>

using v8::Context;
using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Object;
using v8::Value;

// Give these things names in the public namespace so that we can see
// them show up in symbol dumps.
void CloseCallback(uv_handle_t* handle) {}

class ExampleOwnerClass {
 public:
  virtual ~ExampleOwnerClass() {}
};

ExampleOwnerClass example_instance;

void LeakHandle(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  uv_loop_t* loop = node::GetCurrentEventLoop(isolate);
  assert(loop != nullptr);

  uv_timer_t* leaked_timer = new uv_timer_t;
  leaked_timer->close_cb = CloseCallback;

  if (args[0]->IsNumber()) {
    leaked_timer->data =
        reinterpret_cast<void*>(args[0]->IntegerValue(context).FromJust());
  } else {
    leaked_timer->data = &example_instance;
  }

  uv_timer_init(loop, leaked_timer);
  uv_timer_start(leaked_timer, [](uv_timer_t*) {}, 1000, 1000);
  uv_unref(reinterpret_cast<uv_handle_t*>(leaked_timer));
}

void Initialize(v8::Local<v8::Object> exports) {
  NODE_SET_METHOD(exports, "leakHandle", LeakHandle);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, Initialize)
