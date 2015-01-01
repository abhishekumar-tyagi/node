// Copyright Joyent, Inc. and other Node contributors.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so, subject to the
// following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
// NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
// USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef SRC_REQ_WRAP_H_
#define SRC_REQ_WRAP_H_

#include "async-wrap.h"
#include "async-wrap-inl.h"
#include "env.h"
#include "env-inl.h"
#include "queue.h"
#include "util.h"

namespace node {

template <typename T>
class ReqWrap : public AsyncWrap {
 public:
  ReqWrap(Environment* env,
          v8::Handle<v8::Object> object,
          AsyncWrap::ProviderType provider)
      : AsyncWrap(env, object, provider) {
    if (env->in_domain())
      object->Set(env->domain_string(), env->domain_array()->Get(0));

    QUEUE_INSERT_TAIL(env->req_wrap_queue(), &req_wrap_queue_);
  }


  ~ReqWrap() override {
    QUEUE_REMOVE(&req_wrap_queue_);
    // Assert that someone has called Dispatched()
    CHECK_EQ(req_.data, this);
    CHECK_EQ(false, persistent().IsEmpty());
    persistent().Reset();
  }

  inline v8::Handle<v8::Value> Fulfill(int argc, v8::Handle<v8::Value>* argv) {
      // if (obj.oncomplete) .oncomplete(argv)
      v8::Local<v8::Value> cb_v = object()->Get(env()->oncomplete_string());
      if (cb_v->IsFunction()) {
          return MakeCallback(cb_v.As<v8::Function>(), argc, argv);
      }
      // else
      //   if (argv[0]) obj.reject(argv[0])
      //   else obj.resolve(argv[1...])
      if (argc == 0)
        return MakeCallback(env()->resolve_string(), 0, NULL);

      if (argv[0]->ToBoolean()->IsTrue()) // if(err) {
        return MakeCallback(env()->reject_string(), 1, argv);

      return MakeCallback(env()->resolve_string(), argc-1, argv+1);
  }

  // Call this after the req has been dispatched.
  void Dispatched() {
    req_.data = this;
  }

  // TODO(bnoordhuis) Make these private.
  QUEUE req_wrap_queue_;
  T req_;  // *must* be last, GetActiveRequests() in node.cc depends on it
};


}  // namespace node


#endif  // SRC_REQ_WRAP_H_
