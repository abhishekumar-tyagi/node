// Copyright 2016 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Flags: --stack-size=150

function foo() {
 "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "a" + "boom"};

try {
  foo()
} catch(e) {}
