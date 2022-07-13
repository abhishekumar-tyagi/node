// Copyright 2020 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

let a = [0];
let l = {
  valueOf: function() {
    Object.freeze(a);
    return 1;
  }
};
a.length = l;
