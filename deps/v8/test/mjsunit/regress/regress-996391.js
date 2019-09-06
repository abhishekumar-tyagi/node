// Copyright 2019 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Flags: --allow-natives-syntax --regexp-interpret-all

assertArrayEquals(["o"], /.(?<!^.)/m.exec("foobar"));
assertArrayEquals(["o"], /.(?<!\b.)/m.exec("foobar"));
assertArrayEquals(["f"], /.(?<!\B.)/m.exec("foobar"));
