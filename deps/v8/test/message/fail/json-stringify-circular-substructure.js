// Copyright 2019 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

const object = {};
object.substructure = {};
object.substructure.key = object.substructure;

JSON.stringify(object);
