// Copyright 2017 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Flags: --allow-natives-syntax

function foo(f){
  f.caller;
}
function bar(f) {
  new foo(f);
}
bar(function() {});
%OptimizeFunctionOnNextCall(bar);
bar(function() {});
