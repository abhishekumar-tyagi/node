// Copyright 2017 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_CCTEST_UNICODE_HELPERS_H_
#define V8_CCTEST_UNICODE_HELPERS_H_

#include "src/unicode.h"

int Ucs2CharLength(unibrow::uchar c);
int Utf8LengthHelper(const char* s);

#endif  // V8_CCTEST_UNICODE_HELPERS_H_
