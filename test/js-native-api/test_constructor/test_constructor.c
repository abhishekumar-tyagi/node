#include <js_native_api.h>
#include "../common.h"

static double value_ = 1;
static double static_value_ = 10;

static napi_value TestDefineClass(napi_env env,
                                  napi_callback_info info) {
  napi_status ret[7];
  napi_value result, return_value, prop_value;

  napi_property_descriptor property_descriptor = {
    "TestDefineClass",
    NULL,
    TestDefineClass,
    NULL,
    NULL,
    NULL,
    napi_enumerable | napi_static,
    NULL};

  ret[0] = napi_define_class(NULL,
                             "TrackedFunction",
                             NAPI_AUTO_LENGTH,
                             TestDefineClass,
                             NULL,
                             1,
                             &property_descriptor,
                             &result);

  ret[1] = napi_define_class(env,
                             NULL,
                             NAPI_AUTO_LENGTH,
                             TestDefineClass,
                             NULL,
                             1,
                             &property_descriptor,
                             &result);

  ret[2] = napi_define_class(env,
                             "TrackedFunction",
                             NAPI_AUTO_LENGTH,
                             NULL,
                             NULL,
                             1,
                             &property_descriptor,
                             &result);

  ret[3] = napi_define_class(env,
                             "TrackedFunction",
                             NAPI_AUTO_LENGTH,
                             TestDefineClass,
                             NULL,
                             1,
                             &property_descriptor,
                             &result);

  ret[4] = napi_define_class(env,
                             "TrackedFunction",
                             NAPI_AUTO_LENGTH,
                             TestDefineClass,
                             NULL,
                             1,
                             NULL,
                             &result);

  ret[5] = napi_define_class(env,
                             "TrackedFunction",
                             NAPI_AUTO_LENGTH,
                             TestDefineClass,
                             NULL,
                             1,
                             &property_descriptor,
                             NULL);

  NAPI_CALL(env, napi_create_object(env, &return_value));

  NAPI_CALL(env, napi_create_string_utf8(env,
                                         NAPI_STATUS_TO_STRING(ret[0]),
                                         NAPI_AUTO_LENGTH,
                                         &prop_value));
  NAPI_CALL(env, napi_set_named_property(env,
                                         return_value,
                                         "envIsNull",
                                         prop_value));

  NAPI_CALL(env, napi_create_string_utf8(env,
                                         NAPI_STATUS_TO_STRING(ret[1]),
                                         NAPI_AUTO_LENGTH,
                                         &prop_value));
  NAPI_CALL(env, napi_set_named_property(env,
                                         return_value,
                                         "nameIsNull",
                                         prop_value));

  NAPI_CALL(env, napi_create_string_utf8(env,
                                         NAPI_STATUS_TO_STRING(ret[2]),
                                         NAPI_AUTO_LENGTH,
                                         &prop_value));
  NAPI_CALL(env, napi_set_named_property(env,
                                         return_value,
                                         "cbIsNull",
                                         prop_value));

  NAPI_CALL(env, napi_create_string_utf8(env,
                                         NAPI_STATUS_TO_STRING(ret[3]),
                                         NAPI_AUTO_LENGTH,
                                         &prop_value));
  NAPI_CALL(env, napi_set_named_property(env,
                                         return_value,
                                         "cbDataIsNull",
                                         prop_value));

  NAPI_CALL(env, napi_create_string_utf8(env,
                                         NAPI_STATUS_TO_STRING(ret[4]),
                                         NAPI_AUTO_LENGTH,
                                         &prop_value));
  NAPI_CALL(env, napi_set_named_property(env,
                                         return_value,
                                         "propertiesIsNull",
                                         prop_value));

  NAPI_CALL(env, napi_create_string_utf8(env,
                                         NAPI_STATUS_TO_STRING(ret[5]),
                                         NAPI_AUTO_LENGTH,
                                         &prop_value));
  NAPI_CALL(env, napi_set_named_property(env,
                                         return_value,
                                         "resultIsNull",
                                         prop_value));

  return return_value;
}

static napi_value GetValue(napi_env env, napi_callback_info info) {
  size_t argc = 0;
  NAPI_CALL(env, napi_get_cb_info(env, info, &argc, NULL, NULL, NULL));

  NAPI_ASSERT(env, argc == 0, "Wrong number of arguments");

  napi_value number;
  NAPI_CALL(env, napi_create_double(env, value_, &number));

  return number;
}

static napi_value SetValue(napi_env env, napi_callback_info info) {
  size_t argc = 1;
  napi_value args[1];
  NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, NULL, NULL));

  NAPI_ASSERT(env, argc == 1, "Wrong number of arguments");

  NAPI_CALL(env, napi_get_value_double(env, args[0], &value_));

  return NULL;
}

static napi_value Echo(napi_env env, napi_callback_info info) {
  size_t argc = 1;
  napi_value args[1];
  NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, NULL, NULL));

  NAPI_ASSERT(env, argc == 1, "Wrong number of arguments");

  return args[0];
}

static napi_value New(napi_env env, napi_callback_info info) {
  napi_value _this;
  NAPI_CALL(env, napi_get_cb_info(env, info, NULL, NULL, &_this, NULL));

  return _this;
}

static napi_value GetStaticValue(napi_env env, napi_callback_info info) {
  size_t argc = 0;
  NAPI_CALL(env, napi_get_cb_info(env, info, &argc, NULL, NULL, NULL));

  NAPI_ASSERT(env, argc == 0, "Wrong number of arguments");

  napi_value number;
  NAPI_CALL(env, napi_create_double(env, static_value_, &number));

  return number;
}


static napi_value NewExtra(napi_env env, napi_callback_info info) {
  napi_value _this;
  NAPI_CALL(env, napi_get_cb_info(env, info, NULL, NULL, &_this, NULL));

  return _this;
}

EXTERN_C_START
napi_value Init(napi_env env, napi_value exports) {
  napi_value number, cons;
  NAPI_CALL(env, napi_create_double(env, value_, &number));

  NAPI_CALL(env, napi_define_class(
      env, "MyObject_Extra", 8, NewExtra, NULL, 0, NULL, &cons));

  napi_property_descriptor properties[] = {
    { "echo", NULL, Echo, NULL, NULL, NULL, napi_enumerable, NULL },
    { "readwriteValue", NULL, NULL, NULL, NULL, number,
        napi_enumerable | napi_writable, NULL },
    { "readonlyValue", NULL, NULL, NULL, NULL, number, napi_enumerable,
        NULL },
    { "hiddenValue", NULL, NULL, NULL, NULL, number, napi_default, NULL },
    { "readwriteAccessor1", NULL, NULL, GetValue, SetValue, NULL, napi_default,
        NULL },
    { "readwriteAccessor2", NULL, NULL, GetValue, SetValue, NULL,
        napi_writable, NULL },
    { "readonlyAccessor1", NULL, NULL, GetValue, NULL, NULL, napi_default,
        NULL },
    { "readonlyAccessor2", NULL, NULL, GetValue, NULL, NULL, napi_writable,
        NULL },
    { "staticReadonlyAccessor1", NULL, NULL, GetStaticValue, NULL, NULL,
        napi_default | napi_static, NULL},
    { "constructorName", NULL, NULL, NULL, NULL, cons,
        napi_enumerable | napi_static, NULL },
    { "TestDefineClass", NULL, TestDefineClass, NULL, NULL, NULL,
        napi_enumerable | napi_static, NULL },
  };

  NAPI_CALL(env, napi_define_class(env, "MyObject", NAPI_AUTO_LENGTH, New,
      NULL, sizeof(properties)/sizeof(*properties), properties, &cons));

  return cons;
}
EXTERN_C_END
