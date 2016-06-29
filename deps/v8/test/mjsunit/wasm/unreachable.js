// Copyright 2015 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Flags: --expose-wasm

load("test/mjsunit/wasm/wasm-constants.js");
load("test/mjsunit/wasm/wasm-module-builder.js");

var main = (function () {
  var builder = new WasmModuleBuilder();
  builder.addFunction("main", [kAstStmt])
    .addBody([kExprUnreachable])
    .exportAs("main");

  return builder.instantiate().exports.main;
})();

var exception = "";
try {
    assertEquals(0, main());
} catch(e) {
    print("correctly caught: " + e);
    exception = e;
}
assertEquals("unreachable", exception);
