// Copyright 2017 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Flags: --allow-natives-syntax --harmony-dynamic-import

var ran = false;

async function test() {
  try {
    let namespace = await import('modules-skip-5.js');
    assertSame(namespace.static_life, namespace.dynamic_life);
    assertSame(namespace.relative_static_life, namespace.dynamic_life);
    ran = true;
  } catch(e) {
    %AbortJS('failure: ' + e);
  }
}

test();
%RunMicrotasks();
assertTrue(ran);
