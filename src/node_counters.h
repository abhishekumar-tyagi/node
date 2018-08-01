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

#ifndef SRC_NODE_COUNTERS_H_
#define SRC_NODE_COUNTERS_H_

#if defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS

#include "node_internals.h"

#ifdef HAVE_PERFCTR
#include "node_win32_perfctr_provider.h"
#else
#define NODE_COUNTER_ENABLED() (false)
#define NODE_COUNT_GC_PERCENTTIME(percent)                                     \
  do {                                                                         \
  } while (false)
#define NODE_COUNT_GET_GC_RAWTIME()                                            \
  do {                                                                         \
  } while (false)
#define NODE_COUNT_HTTP_CLIENT_REQUEST()                                       \
  do {                                                                         \
  } while (false)
#define NODE_COUNT_HTTP_CLIENT_RESPONSE()                                      \
  do {                                                                         \
  } while (false)
#define NODE_COUNT_HTTP_SERVER_REQUEST()                                       \
  do {                                                                         \
  } while (false)
#define NODE_COUNT_HTTP_SERVER_RESPONSE()                                      \
  do {                                                                         \
  } while (false)
#define NODE_COUNT_NET_BYTES_RECV(bytes)                                       \
  do {                                                                         \
  } while (false)
#define NODE_COUNT_NET_BYTES_SENT(bytes)                                       \
  do {                                                                         \
  } while (false)
#define NODE_COUNT_PIPE_BYTES_RECV(bytes)                                      \
  do {                                                                         \
  } while (false)
#define NODE_COUNT_PIPE_BYTES_SENT(bytes)                                      \
  do {                                                                         \
  } while (false)
#define NODE_COUNT_SERVER_CONN_CLOSE()                                         \
  do {                                                                         \
  } while (false)
#define NODE_COUNT_SERVER_CONN_OPEN()                                          \
  do {                                                                         \
  } while (false)
#endif

#include "env.h"
#include "v8.h"

namespace node {

void InitPerfCounters(Environment* env, v8::Local<v8::Object> target);
void TermPerfCounters(v8::Local<v8::Object> target);

}  // namespace node

#endif  // defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS

#endif  // SRC_NODE_COUNTERS_H_
