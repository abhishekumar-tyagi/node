// Copyright 2016 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import foo from "modules-skip-1.mjs";
assertEquals(42, foo);

import {default as gaga} from "modules-skip-1.mjs";
assertEquals(42, gaga);
