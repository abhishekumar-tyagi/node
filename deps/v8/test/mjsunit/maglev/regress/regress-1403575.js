// Copyright 2022 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Flags: --allow-natives-syntax

function f(y) {
  const x = y % y;
  return 1 / x;
}
%PrepareFunctionForOptimization(f);
assertEquals(f(2), Infinity);
%OptimizeMaglevOnNextCall(f);
assertEquals(f(-2), -Infinity);
