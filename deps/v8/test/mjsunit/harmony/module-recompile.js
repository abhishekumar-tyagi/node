// Copyright 2012 the V8 project authors. All rights reserved.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials provided
//       with the distribution.
//     * Neither the name of Google Inc. nor the names of its
//       contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// Flags: --harmony-modules

// Test that potential recompilation of the global scope does not screw up.

"use strict";

var N = 1e5;  // Number of loop iterations that trigger optimization.

module A {
  export var x = 1
  export function f() { return x }
}
var f = A.f

assertEquals(1, A.x)
assertEquals(1, A.f())
assertEquals(1, f())

A.x = 2

assertEquals(2, A.x)
assertEquals(2, A.f())
assertEquals(2, f())

for (var i = 0; i < N; i++) {
  if (i > N) print("impossible");
}

assertEquals(2, A.x)
assertEquals(2, A.f())
assertEquals(2, f())


// Same test with loop inside a module.

module B {
  module A {
    export var x = 1
    export function f() { return x }
  }
  var f = A.f

  assertEquals(1, A.x)
  assertEquals(1, A.f())
  assertEquals(1, f())

  A.x = 2

  assertEquals(2, A.x)
  assertEquals(2, A.f())
  assertEquals(2, f())

  for (var i = 0; i < N; i++) {
    if (i > N) print("impossible");
  }

  assertEquals(2, A.x)
  assertEquals(2, A.f())
  assertEquals(2, f())
}
