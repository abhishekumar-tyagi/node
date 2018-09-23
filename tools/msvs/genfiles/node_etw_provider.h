//**********************************************************************`
//* This is an include file generated by Message Compiler.             *`
//*                                                                    *`
//* Copyright (c) Microsoft Corporation. All Rights Reserved.          *`
//**********************************************************************`
#pragma once
//+
// Provider NodeJS-ETW-provider Event Count 12
//+
EXTERN_C __declspec(selectany) const GUID NODE_ETW_PROVIDER = {0x77754e9b, 0x264b, 0x4d8d, {0xb9, 0x81, 0xe4, 0x13, 0x5c, 0x1e, 0xcb, 0x0c}};

//
// Opcodes
//
#define NODE_ETW_PROVIDER_OPCODE_NODE_HTTP_SERVER_REQUEST 0xa
#define NODE_ETW_PROVIDER_OPCODE_NODE_HTTP_SERVER_RESPONSE 0xb
#define NODE_ETW_PROVIDER_OPCODE_NODE_HTTP_CLIENT_REQUEST 0xc
#define NODE_ETW_PROVIDER_OPCODE_NODE_HTTP_CLIENT_RESPONSE 0xd
#define NODE_ETW_PROVIDER_OPCODE_NODE_NET_SERVER_CONNECTION 0xe
#define NODE_ETW_PROVIDER_OPCODE_NODE_NET_STREAM_END 0xf
#define NODE_ETW_PROVIDER_OPCODE_NODE_GC_START 0x10
#define NODE_ETW_PROVIDER_OPCODE_NODE_GC_DONE 0x11
#define NODE_ETW_PROVIDER_OPCODE_NODE_V8SYMBOL_REMOVE 0x15
#define NODE_ETW_PROVIDER_OPCODE_NODE_V8SYMBOL_MOVE 0x16
#define NODE_ETW_PROVIDER_OPCODE_NODE_V8SYMBOL_RESET 0x17
#define JSCRIPT_METHOD_METHODLOAD_OPCODE 0xa

//
// Tasks
//
#define JSCRIPT_METHOD_RUNTIME_TASK 0x1

//
// Event Descriptors
//
EXTERN_C __declspec(selectany) const EVENT_DESCRIPTOR NODE_HTTP_SERVER_REQUEST_EVENT = {0x1, 0x0, 0x0, 0x4, 0xa, 0x0, 0x0};
#define NODE_HTTP_SERVER_REQUEST_EVENT_value 0x1
EXTERN_C __declspec(selectany) const EVENT_DESCRIPTOR NODE_HTTP_SERVER_RESPONSE_EVENT = {0x2, 0x0, 0x0, 0x4, 0xb, 0x0, 0x0};
#define NODE_HTTP_SERVER_RESPONSE_EVENT_value 0x2
EXTERN_C __declspec(selectany) const EVENT_DESCRIPTOR NODE_HTTP_CLIENT_REQUEST_EVENT = {0x3, 0x0, 0x0, 0x4, 0xc, 0x0, 0x0};
#define NODE_HTTP_CLIENT_REQUEST_EVENT_value 0x3
EXTERN_C __declspec(selectany) const EVENT_DESCRIPTOR NODE_HTTP_CLIENT_RESPONSE_EVENT = {0x4, 0x0, 0x0, 0x4, 0xd, 0x0, 0x0};
#define NODE_HTTP_CLIENT_RESPONSE_EVENT_value 0x4
EXTERN_C __declspec(selectany) const EVENT_DESCRIPTOR NODE_NET_SERVER_CONNECTION_EVENT = {0x5, 0x0, 0x0, 0x4, 0xe, 0x0, 0x0};
#define NODE_NET_SERVER_CONNECTION_EVENT_value 0x5
EXTERN_C __declspec(selectany) const EVENT_DESCRIPTOR NODE_NET_STREAM_END_EVENT = {0x6, 0x0, 0x0, 0x4, 0xf, 0x0, 0x0};
#define NODE_NET_STREAM_END_EVENT_value 0x6
EXTERN_C __declspec(selectany) const EVENT_DESCRIPTOR NODE_GC_START_EVENT = {0x7, 0x0, 0x0, 0x4, 0x10, 0x0, 0x0};
#define NODE_GC_START_EVENT_value 0x7
EXTERN_C __declspec(selectany) const EVENT_DESCRIPTOR NODE_GC_DONE_EVENT = {0x8, 0x0, 0x0, 0x4, 0x11, 0x0, 0x0};
#define NODE_GC_DONE_EVENT_value 0x8
EXTERN_C __declspec(selectany) const EVENT_DESCRIPTOR MethodLoad = {0x9, 0x0, 0x0, 0x4, 0xa, 0x1, 0x0};
#define MethodLoad_value 0x9
EXTERN_C __declspec(selectany) const EVENT_DESCRIPTOR NODE_V8SYMBOL_REMOVE_EVENT = {0x15, 0x0, 0x0, 0x4, 0x15, 0x0, 0x0};
#define NODE_V8SYMBOL_REMOVE_EVENT_value 0x15
EXTERN_C __declspec(selectany) const EVENT_DESCRIPTOR NODE_V8SYMBOL_MOVE_EVENT = {0x16, 0x0, 0x0, 0x4, 0x16, 0x0, 0x0};
#define NODE_V8SYMBOL_MOVE_EVENT_value 0x16
EXTERN_C __declspec(selectany) const EVENT_DESCRIPTOR NODE_V8SYMBOL_RESET_EVENT = {0x17, 0x0, 0x0, 0x4, 0x17, 0x0, 0x0};
#define NODE_V8SYMBOL_RESET_EVENT_value 0x17
