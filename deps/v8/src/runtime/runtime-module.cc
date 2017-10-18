// Copyright 2016 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/runtime/runtime-utils.h"

#include "src/arguments.h"
#include "src/counters.h"
#include "src/objects-inl.h"

namespace v8 {
namespace internal {

RUNTIME_FUNCTION(Runtime_DynamicImportCall) {
  HandleScope scope(isolate);
  DCHECK_EQ(2, args.length());
  CONVERT_ARG_HANDLE_CHECKED(JSFunction, function, 0);
  CONVERT_ARG_HANDLE_CHECKED(Object, specifier, 1);

  Handle<Script> script(Script::cast(function->shared()->script()));

  while (script->eval_from_shared()->IsSharedFunctionInfo()) {
    script = handle(
        Script::cast(
            SharedFunctionInfo::cast(script->eval_from_shared())->script()),
        isolate);
  }

  // TODO(gsathya): Check if script name is a string before casting
  // and return undefined if not.
  Handle<String> source_url(String::cast(script->name()));
  RETURN_RESULT_OR_FAILURE(
      isolate,
      isolate->RunHostImportModuleDynamicallyCallback(source_url, specifier));
}

RUNTIME_FUNCTION(Runtime_GetModuleNamespace) {
  HandleScope scope(isolate);
  DCHECK_EQ(1, args.length());
  CONVERT_SMI_ARG_CHECKED(module_request, 0);
  Handle<Module> module(isolate->context()->module());
  return *Module::GetModuleNamespace(module, module_request);
}

RUNTIME_FUNCTION(Runtime_LoadModuleVariable) {
  HandleScope scope(isolate);
  DCHECK_EQ(1, args.length());
  CONVERT_SMI_ARG_CHECKED(index, 0);
  Handle<Module> module(isolate->context()->module());
  return *Module::LoadVariable(module, index);
}

RUNTIME_FUNCTION(Runtime_StoreModuleVariable) {
  HandleScope scope(isolate);
  DCHECK_EQ(2, args.length());
  CONVERT_SMI_ARG_CHECKED(index, 0);
  CONVERT_ARG_HANDLE_CHECKED(Object, value, 1);
  Handle<Module> module(isolate->context()->module());
  Module::StoreVariable(module, index, value);
  return isolate->heap()->undefined_value();
}

}  // namespace internal
}  // namespace v8
