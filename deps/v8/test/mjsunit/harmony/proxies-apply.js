// Copyright 2015 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Flags: --harmony-proxies --harmony-reflect

(function testNonCallable() {
  var proxy = new Proxy({},{});
  assertThrows(function(){ proxy() }, TypeError);

  var proxy2 = new Proxy(proxy, {});
  assertThrows(function(){ proxy2() }, TypeError);
})();

(function testCallProxyFallbackNoArguments() {
  var called = false;
  var target = function() {
    called = true;
  }
  var proxy = new Proxy(target, {});
  assertFalse(called);
  proxy();
  assertTrue(called);

  called = false;
  var proxy2 = new Proxy(proxy, {});
  assertFalse(called);
  proxy2();
  assertTrue(called);
})();

(function testCallProxyFallback1Argument() {
  var called = false;
  var target = function(a) {
    called = true;
    assertEquals('1', a);
  }
  var proxy = new Proxy(target, {});
  assertFalse(called);
  proxy('1');
  assertTrue(called);
})();

(function testCallProxyFallback2Arguments() {
  var called = false;
  var target = function(a, b) {
    called = true;
    assertEquals('1', a);
    assertEquals('2', b);
  }
  var proxy = new Proxy(target, {});
  assertFalse(called);
  proxy('1', '2');
  assertTrue(called);
})();

(function testCallProxyFallbackChangedReceiver() {
  var apply_receiver = {receiver:true};
  var seen_receiver = undefined;
  var target = function() {
    seen_receiver = this;
  }
  var proxy = new Proxy(target, {});
  assertEquals(undefined, seen_receiver);
  Reflect.apply(proxy, apply_receiver, [1,2,3,4]);
  assertSame(apply_receiver, seen_receiver);
})();

(function testCallProxyTrap() {
  var called_target = false;
  var called_handler = false;
  var target = function(a, b) {
    called_target = true;
    assertEquals(1, a);
    assertEquals(2, b);
  }
  var handler = {
    apply: function(target, this_arg, args) {
      target.apply(this_arg, args);
      called_handler = true;
    }
  }
  var proxy = new Proxy(target, handler);
  assertFalse(called_target);
  assertFalse(called_handler);
  Reflect.apply(proxy, {rec:1}, [1,2]);
  assertTrue(called_target);
  assertTrue(called_handler);
})();
