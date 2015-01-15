// Copyright 2008 The Closure Linter Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS-IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * @fileoverview Numbers should come before letters.
 *
 */

goog.provide('xa'); // GOOG_PROVIDES_NOT_ALPHABETIZED
goog.provide('x1');
goog.provide('xb');

goog.require('dummy.aa'); // GOOG_REQUIRES_NOT_ALPHABETIZED
goog.require('dummy.a1');
goog.require('dummy.ab');

dummy.aa.a;
dummy.a1.a;
dummy.ab.a;

