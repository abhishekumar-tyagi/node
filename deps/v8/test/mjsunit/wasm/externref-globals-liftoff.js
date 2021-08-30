// Copyright 2020 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Flags: --experimental-wasm-reftypes --expose-gc --liftoff
// Flags: --no-wasm-tier-up --experimental-liftoff-extern-ref

d8.file.execute("test/mjsunit/wasm/externref-globals.js");
