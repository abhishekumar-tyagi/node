// Copyright 2018 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WASM_ATOMICOP_UTILS_H
#define WASM_ATOMICOP_UTILS_H

#include "src/objects-inl.h"
#include "test/cctest/cctest.h"
#include "test/cctest/compiler/value-helper.h"
#include "test/cctest/wasm/wasm-run-utils.h"

namespace v8 {
namespace internal {
namespace wasm {

typedef uint64_t (*Uint64BinOp)(uint64_t, uint64_t);
typedef uint32_t (*Uint32BinOp)(uint32_t, uint32_t);
typedef uint16_t (*Uint16BinOp)(uint16_t, uint16_t);
typedef uint8_t (*Uint8BinOp)(uint8_t, uint8_t);

template <typename T>
T Add(T a, T b) {
  return a + b;
}

template <typename T>
T Sub(T a, T b) {
  return a - b;
}

template <typename T>
T And(T a, T b) {
  return a & b;
}

template <typename T>
T Or(T a, T b) {
  return a | b;
}

template <typename T>
T Xor(T a, T b) {
  return a ^ b;
}

template <typename T>
T Exchange(T a, T b) {
  return b;
}

template <typename T>
T CompareExchange(T initial, T a, T b) {
  if (initial == a) return b;
  return a;
}

}  // namespace wasm
}  // namespace internal
}  // namespace v8

#endif
