// Copyright 2024 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Flags: --allow-natives-syntax

function fct() {
  var obj = {};
  obj.length = (new Uint8ClampedArray().length);
}

%PrepareFunctionForOptimization(fct);
fct();
%OptimizeFunctionOnNextCall(fct);
fct();
