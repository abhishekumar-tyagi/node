// Copyright 2015 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

var x = 0;
function a() {
  eval("");
  return (function() {
    eval("");
    return (function() {
      eval("");
      return (function() {
        eval("");
        return (function() {
          eval("");
          return (function() {
            eval("");
            return (function() {
              eval("");
              return (function() {
                eval("");
                return x;
              })();
            }) ();
          })();
        })();
      })();
    })();
  })();
}
assertEquals(a(), 0);
