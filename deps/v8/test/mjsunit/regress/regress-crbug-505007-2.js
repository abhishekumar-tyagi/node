// Copyright 2015 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Flags: --stack-size=100 --allow-natives-syntax

function g() {}
function f() {
  try {
    f();
  } catch(e) {
    %ExecuteInDebugContext(g);
  }
}
f();
