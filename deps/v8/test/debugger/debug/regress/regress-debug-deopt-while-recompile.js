// Copyright 2013 the V8 project authors. All rights reserved.
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


Debug = debug.Debug;

function listener(event, exec_state, event_data, data) {
  if (event != Debug.DebugEvent.Break) return;
  try {
    assertEquals("foo", exec_state.frame(0).evaluate("bar").value());
  } catch (e) {
    exception = e;
  };
  listened++;
};

var exception = null;
var listened = 0;

var f = function() {
  var bar = "foo";
  var baz = bar;  // Break point should be here.
  return bar;
};

var g = function() {
  var bar = "foo";
  var baz = bar;  // Break point should be here.
  return bar;
};

%PrepareFunctionForOptimization(f);
%PrepareFunctionForOptimization(g);

f();
f();
g();
g();

// Mark with builtin.
%OptimizeFunctionOnNextCall(f);
if (%IsConcurrentRecompilationSupported()) {
  %OptimizeFunctionOnNextCall(g, "concurrent");
}

// Activate debugger.
Debug.setListener(listener);

 // Set break point.
Debug.setBreakPoint(f, 2, 0);
Debug.setBreakPoint(g, 2, 0);

// Trigger break point.
f();
g();

// Assert that break point is set at expected location.
assertTrue(Debug.showBreakPoints(f).indexOf("var baz = [B0]bar;") > 0);
assertTrue(Debug.showBreakPoints(g).indexOf("var baz = [B0]bar;") > 0);

assertEquals(2, listened);
assertNull(exception);
