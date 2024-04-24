// Copyright 2024 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Flags: --const-tracking-let --allow-natives-syntax
// Flags: --turbofan --no-always-turbofan --maglev --no-stress-maglev
// Flags: --sparkplug --no-always-sparkplug

%RuntimeEvaluateREPL('let y = 42;');

%RuntimeEvaluateREPL('function foo() { return y; }');

%PrepareFunctionForOptimization(foo);
foo();

// This will assert that the value initialization was tracked correctly.
%OptimizeFunctionOnNextCall(foo);
assertEquals(42, foo());
assertOptimized(foo);
