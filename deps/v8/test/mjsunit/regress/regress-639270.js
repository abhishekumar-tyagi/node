// Copyright 2016 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Flags: --allow-natives-syntax --es-staging --ignition --turbo

"use strict";

var g = (async () => { return JSON.stringify() });

g();
g();
%OptimizeFunctionOnNextCall(g);
g();
