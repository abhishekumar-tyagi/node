#include "env-inl.h"
#include "memory_tracker.h"
#include "node.h"
#include "node_i18n.h"
#include "node_native_module_env.h"
#include "node_options.h"
#include "snapshot_support-inl.h"
#include "util-inl.h"

namespace node {

using v8::Context;
using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Number;
using v8::Object;
using v8::Value;

static void HasCachedBuiltins(const FunctionCallbackInfo<Value>& args) {
  args.GetReturnValue().Set(native_module::has_code_cache);
}

// The config binding is used to provide an internal view of compile time
// config options that are required internally by lib/*.js code. This is an
// alternative to dropping additional properties onto the process object as
// has been the practice previously in node.cc.

// Command line arguments are already accessible in the JS land via
// require('internal/options').getOptionValue('--some-option'). Do not add them
// here.
static void Initialize(Local<Object> target,
                       Local<Value> unused,
                       Local<Context> context,
                       void* priv) {
  Environment* env = Environment::GetCurrent(context);
  Isolate* isolate = env->isolate();

#if defined(DEBUG) && DEBUG
  READONLY_TRUE_PROPERTY(target, "isDebugBuild");
#else
  READONLY_FALSE_PROPERTY(target, "isDebugBuild");
#endif  // defined(DEBUG) && DEBUG

#if HAVE_OPENSSL
  READONLY_TRUE_PROPERTY(target, "hasOpenSSL");
#else
  READONLY_FALSE_PROPERTY(target, "hasOpenSSL");
#endif  // HAVE_OPENSSL

#ifdef NODE_FIPS_MODE
  READONLY_TRUE_PROPERTY(target, "fipsMode");
#endif

#ifdef NODE_HAVE_I18N_SUPPORT

  READONLY_TRUE_PROPERTY(target, "hasIntl");

#ifdef NODE_HAVE_SMALL_ICU
  READONLY_TRUE_PROPERTY(target, "hasSmallICU");
#endif  // NODE_HAVE_SMALL_ICU

#if NODE_USE_V8_PLATFORM
  READONLY_TRUE_PROPERTY(target, "hasTracing");
#endif

#if !defined(NODE_WITHOUT_NODE_OPTIONS)
  READONLY_TRUE_PROPERTY(target, "hasNodeOptions");
#endif

#endif  // NODE_HAVE_I18N_SUPPORT

#if HAVE_INSPECTOR
  READONLY_TRUE_PROPERTY(target, "hasInspector");
#else
  READONLY_FALSE_PROPERTY(target, "hasInspector");
#endif

// configure --no-browser-globals
#ifdef NODE_NO_BROWSER_GLOBALS
  READONLY_TRUE_PROPERTY(target, "noBrowserGlobals");
#else
  READONLY_FALSE_PROPERTY(target, "noBrowserGlobals");
#endif  // NODE_NO_BROWSER_GLOBALS

  READONLY_PROPERTY(target,
                    "bits",
                    Number::New(isolate, 8 * sizeof(intptr_t)));

#if defined HAVE_DTRACE || defined HAVE_ETW
  READONLY_TRUE_PROPERTY(target, "hasDtrace");
#endif

  // Make this a method instead of a constant in order for snapshotted builts
  // to correctly report this.
  env->SetMethodNoSideEffect(target, "hasCachedBuiltins", HasCachedBuiltins);
}  // InitConfig

static ExternalReferences external_references {
  __FILE__,
  HasCachedBuiltins
};

}  // namespace node

NODE_MODULE_CONTEXT_AWARE_INTERNAL(config, node::Initialize)
