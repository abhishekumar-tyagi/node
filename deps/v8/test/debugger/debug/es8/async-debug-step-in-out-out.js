// Copyright 2016 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

var Debug = debug.Debug;
var step_count = 0;

function listener(event, execState, eventData, data) {
  if (event != Debug.DebugEvent.Break) return;
  try {
    var line = execState.frame(0).sourceLineText();
    print(line);
    var [match, expected_count, step] = /\/\/ B(\d) (\w+)$/.exec(line);
    assertEquals(step_count++, parseInt(expected_count));
    if (step != "Continue") execState.prepareStep(Debug.StepAction[step]);
  } catch (e) {
    print(e, e.stack);
    quit(1);
  }
}

Debug.setListener(listener);

var late_resolve;

function g() {
  return new Promise( // B2 StepOut
    function(res, rej) {
      late_resolve = res;
    }
  );
}

async function f() {
  var a = 1;
  debugger;        // B0 StepOver
  a +=
       await       // B1 StepInto
             g();
  return a;        // B3 Continue
}

f();

late_resolve(3);

%PerformMicrotaskCheckpoint();

assertEquals(4, step_count);
