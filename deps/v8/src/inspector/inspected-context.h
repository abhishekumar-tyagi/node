// Copyright 2016 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_INSPECTOR_INSPECTED_CONTEXT_H_
#define V8_INSPECTOR_INSPECTED_CONTEXT_H_

#include <unordered_map>
#include <unordered_set>

#include "src/base/macros.h"
#include "src/inspector/string-16.h"

#include "include/v8.h"

namespace v8_inspector {

class InjectedScript;
class InjectedScriptHost;
class V8ContextInfo;
class V8InspectorImpl;

class InspectedContext {
 public:
  ~InspectedContext();

  static int contextId(v8::Local<v8::Context>);

  v8::Local<v8::Context> context() const;
  int contextId() const { return m_contextId; }
  int contextGroupId() const { return m_contextGroupId; }
  String16 origin() const { return m_origin; }
  String16 humanReadableName() const { return m_humanReadableName; }
  String16 auxData() const { return m_auxData; }

  bool isReported(int sessionId) const;
  void setReported(int sessionId, bool reported);

  v8::Isolate* isolate() const;
  V8InspectorImpl* inspector() const { return m_inspector; }

  InjectedScript* getInjectedScript(int sessionId);
  bool createInjectedScript(int sessionId);
  void discardInjectedScript(int sessionId);

 private:
  friend class V8InspectorImpl;
  InspectedContext(V8InspectorImpl*, const V8ContextInfo&, int contextId);

  class WeakCallbackData;

  V8InspectorImpl* m_inspector;
  v8::Global<v8::Context> m_context;
  int m_contextId;
  int m_contextGroupId;
  const String16 m_origin;
  const String16 m_humanReadableName;
  const String16 m_auxData;
  std::unordered_set<int> m_reportedSessionIds;
  std::unordered_map<int, std::unique_ptr<InjectedScript>> m_injectedScripts;
  WeakCallbackData* m_weakCallbackData;

  DISALLOW_COPY_AND_ASSIGN(InspectedContext);
};

}  // namespace v8_inspector

#endif  // V8_INSPECTOR_INSPECTED_CONTEXT_H_
