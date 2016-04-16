// Copyright 2016 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Flags: --allow-natives-syntax --no-lazy
// Flags: --harmony-destructuring-bind

"use strict";
eval();
var f = ({x}) => { };
%OptimizeFunctionOnNextCall(f);
assertThrows(f);
