﻿/* Copyright (c) 2012-2017 The ANTLR Project. All rights reserved.
 * Use of this file is governed by the BSD 3-clause license that
 * can be found in the LICENSE.txt file in the project root.
 */

#include "atn/BlockEndState.h"

using namespace antlr4::atn;

BlockEndState::BlockEndState() : startState(nullptr) {}

size_t BlockEndState::getStateType() { return BLOCK_END; }
