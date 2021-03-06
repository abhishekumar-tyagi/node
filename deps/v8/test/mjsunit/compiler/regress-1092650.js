// Copyright 2020 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Flags: --allow-natives-syntax

// Create map with HeapNumber in field 'a'
({a: 2**30});

function foo() {
  return foo.arguments[0];
}

function main() {
  foo({a: 42});
}

%PrepareFunctionForOptimization(foo);
%PrepareFunctionForOptimization(main);
main();
main();
%OptimizeFunctionOnNextCall(main);
main();
