// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
//---------------------------------------------------------------------------------
//
// Generated Header File.  Do not edit by hand.
//    This file contains the state table for the ICU Rule Based Break Iterator
//    rule parser.
//    It is generated by the Perl script "rbbicst.pl" from
//    the rule parser state definitions file "rbbirpt.txt".
//
//   Copyright (C) 2002-2016 International Business Machines Corporation
//   and others. All rights reserved.
//
//---------------------------------------------------------------------------------
#ifndef RBBIRPT_H
#define RBBIRPT_H

#include "unicode/utypes.h"

U_NAMESPACE_BEGIN
//
// Character classes for RBBI rule scanning.
//
    static const uint8_t kRuleSet_digit_char = 128;
    static const uint8_t kRuleSet_name_char = 129;
    static const uint8_t kRuleSet_name_start_char = 130;
    static const uint8_t kRuleSet_rule_char = 131;
    static const uint8_t kRuleSet_white_space = 132;


enum RBBI_RuleParseAction {
    doCheckVarDef,
    doDotAny,
    doEndAssign,
    doEndOfRule,
    doEndVariableName,
    doExit,
    doExprCatOperator,
    doExprFinished,
    doExprOrOperator,
    doExprRParen,
    doExprStart,
    doLParen,
    doNOP,
    doNoChain,
    doOptionEnd,
    doOptionStart,
    doReverseDir,
    doRuleChar,
    doRuleError,
    doRuleErrorAssignExpr,
    doScanUnicodeSet,
    doSlash,
    doStartAssign,
    doStartTagValue,
    doStartVariableName,
    doTagDigit,
    doTagExpectedError,
    doTagValue,
    doUnaryOpPlus,
    doUnaryOpQuestion,
    doUnaryOpStar,
    doVariableNameExpectedErr,
    rbbiLastAction};

//-------------------------------------------------------------------------------
//
//  RBBIRuleTableEl    represents the structure of a row in the transition table
//                     for the rule parser state machine.
//-------------------------------------------------------------------------------
struct RBBIRuleTableEl {
    RBBI_RuleParseAction          fAction;
    uint8_t                       fCharClass;       // 0-127:    an individual ASCII character
                                                    // 128-255:  character class index
    uint8_t                       fNextState;       // 0-250:    normal next-stat numbers
                                                    // 255:      pop next-state from stack.
    uint8_t                       fPushState;
    UBool                         fNextChar;
};

static const struct RBBIRuleTableEl gRuleParseStateTable[] = {
    {doNOP, 0, 0, 0, TRUE}
    , {doExprStart, 254, 29, 9, FALSE}     //  1      start
    , {doNOP, 132, 1,0,  TRUE}     //  2
    , {doNoChain, 94 /* ^ */, 12, 9, TRUE}     //  3
    , {doExprStart, 36 /* $ */, 88, 98, FALSE}     //  4
    , {doNOP, 33 /* ! */, 19,0,  TRUE}     //  5
    , {doNOP, 59 /* ; */, 1,0,  TRUE}     //  6
    , {doNOP, 252, 0,0,  FALSE}     //  7
    , {doExprStart, 255, 29, 9, FALSE}     //  8
    , {doEndOfRule, 59 /* ; */, 1,0,  TRUE}     //  9      break-rule-end
    , {doNOP, 132, 9,0,  TRUE}     //  10
    , {doRuleError, 255, 103,0,  FALSE}     //  11
    , {doExprStart, 254, 29,0,  FALSE}     //  12      start-after-caret
    , {doNOP, 132, 12,0,  TRUE}     //  13
    , {doRuleError, 94 /* ^ */, 103,0,  FALSE}     //  14
    , {doExprStart, 36 /* $ */, 88, 37, FALSE}     //  15
    , {doRuleError, 59 /* ; */, 103,0,  FALSE}     //  16
    , {doRuleError, 252, 103,0,  FALSE}     //  17
    , {doExprStart, 255, 29,0,  FALSE}     //  18
    , {doNOP, 33 /* ! */, 21,0,  TRUE}     //  19      rev-option
    , {doReverseDir, 255, 28, 9, FALSE}     //  20
    , {doOptionStart, 130, 23,0,  TRUE}     //  21      option-scan1
    , {doRuleError, 255, 103,0,  FALSE}     //  22
    , {doNOP, 129, 23,0,  TRUE}     //  23      option-scan2
    , {doOptionEnd, 255, 25,0,  FALSE}     //  24
    , {doNOP, 59 /* ; */, 1,0,  TRUE}     //  25      option-scan3
    , {doNOP, 132, 25,0,  TRUE}     //  26
    , {doRuleError, 255, 103,0,  FALSE}     //  27
    , {doExprStart, 255, 29, 9, FALSE}     //  28      reverse-rule
    , {doRuleChar, 254, 38,0,  TRUE}     //  29      term
    , {doNOP, 132, 29,0,  TRUE}     //  30
    , {doRuleChar, 131, 38,0,  TRUE}     //  31
    , {doNOP, 91 /* [ */, 94, 38, FALSE}     //  32
    , {doLParen, 40 /* ( */, 29, 38, TRUE}     //  33
    , {doNOP, 36 /* $ */, 88, 37, FALSE}     //  34
    , {doDotAny, 46 /* . */, 38,0,  TRUE}     //  35
    , {doRuleError, 255, 103,0,  FALSE}     //  36
    , {doCheckVarDef, 255, 38,0,  FALSE}     //  37      term-var-ref
    , {doNOP, 132, 38,0,  TRUE}     //  38      expr-mod
    , {doUnaryOpStar, 42 /* * */, 43,0,  TRUE}     //  39
    , {doUnaryOpPlus, 43 /* + */, 43,0,  TRUE}     //  40
    , {doUnaryOpQuestion, 63 /* ? */, 43,0,  TRUE}     //  41
    , {doNOP, 255, 43,0,  FALSE}     //  42
    , {doExprCatOperator, 254, 29,0,  FALSE}     //  43      expr-cont
    , {doNOP, 132, 43,0,  TRUE}     //  44
    , {doExprCatOperator, 131, 29,0,  FALSE}     //  45
    , {doExprCatOperator, 91 /* [ */, 29,0,  FALSE}     //  46
    , {doExprCatOperator, 40 /* ( */, 29,0,  FALSE}     //  47
    , {doExprCatOperator, 36 /* $ */, 29,0,  FALSE}     //  48
    , {doExprCatOperator, 46 /* . */, 29,0,  FALSE}     //  49
    , {doExprCatOperator, 47 /* / */, 55,0,  FALSE}     //  50
    , {doExprCatOperator, 123 /* { */, 67,0,  TRUE}     //  51
    , {doExprOrOperator, 124 /* | */, 29,0,  TRUE}     //  52
    , {doExprRParen, 41 /* ) */, 255,0,  TRUE}     //  53
    , {doExprFinished, 255, 255,0,  FALSE}     //  54
    , {doSlash, 47 /* / */, 57,0,  TRUE}     //  55      look-ahead
    , {doNOP, 255, 103,0,  FALSE}     //  56
    , {doExprCatOperator, 254, 29,0,  FALSE}     //  57      expr-cont-no-slash
    , {doNOP, 132, 43,0,  TRUE}     //  58
    , {doExprCatOperator, 131, 29,0,  FALSE}     //  59
    , {doExprCatOperator, 91 /* [ */, 29,0,  FALSE}     //  60
    , {doExprCatOperator, 40 /* ( */, 29,0,  FALSE}     //  61
    , {doExprCatOperator, 36 /* $ */, 29,0,  FALSE}     //  62
    , {doExprCatOperator, 46 /* . */, 29,0,  FALSE}     //  63
    , {doExprOrOperator, 124 /* | */, 29,0,  TRUE}     //  64
    , {doExprRParen, 41 /* ) */, 255,0,  TRUE}     //  65
    , {doExprFinished, 255, 255,0,  FALSE}     //  66
    , {doNOP, 132, 67,0,  TRUE}     //  67      tag-open
    , {doStartTagValue, 128, 70,0,  FALSE}     //  68
    , {doTagExpectedError, 255, 103,0,  FALSE}     //  69
    , {doNOP, 132, 74,0,  TRUE}     //  70      tag-value
    , {doNOP, 125 /* } */, 74,0,  FALSE}     //  71
    , {doTagDigit, 128, 70,0,  TRUE}     //  72
    , {doTagExpectedError, 255, 103,0,  FALSE}     //  73
    , {doNOP, 132, 74,0,  TRUE}     //  74      tag-close
    , {doTagValue, 125 /* } */, 77,0,  TRUE}     //  75
    , {doTagExpectedError, 255, 103,0,  FALSE}     //  76
    , {doExprCatOperator, 254, 29,0,  FALSE}     //  77      expr-cont-no-tag
    , {doNOP, 132, 77,0,  TRUE}     //  78
    , {doExprCatOperator, 131, 29,0,  FALSE}     //  79
    , {doExprCatOperator, 91 /* [ */, 29,0,  FALSE}     //  80
    , {doExprCatOperator, 40 /* ( */, 29,0,  FALSE}     //  81
    , {doExprCatOperator, 36 /* $ */, 29,0,  FALSE}     //  82
    , {doExprCatOperator, 46 /* . */, 29,0,  FALSE}     //  83
    , {doExprCatOperator, 47 /* / */, 55,0,  FALSE}     //  84
    , {doExprOrOperator, 124 /* | */, 29,0,  TRUE}     //  85
    , {doExprRParen, 41 /* ) */, 255,0,  TRUE}     //  86
    , {doExprFinished, 255, 255,0,  FALSE}     //  87
    , {doStartVariableName, 36 /* $ */, 90,0,  TRUE}     //  88      scan-var-name
    , {doNOP, 255, 103,0,  FALSE}     //  89
    , {doNOP, 130, 92,0,  TRUE}     //  90      scan-var-start
    , {doVariableNameExpectedErr, 255, 103,0,  FALSE}     //  91
    , {doNOP, 129, 92,0,  TRUE}     //  92      scan-var-body
    , {doEndVariableName, 255, 255,0,  FALSE}     //  93
    , {doScanUnicodeSet, 91 /* [ */, 255,0,  TRUE}     //  94      scan-unicode-set
    , {doScanUnicodeSet, 112 /* p */, 255,0,  TRUE}     //  95
    , {doScanUnicodeSet, 80 /* P */, 255,0,  TRUE}     //  96
    , {doNOP, 255, 103,0,  FALSE}     //  97
    , {doNOP, 132, 98,0,  TRUE}     //  98      assign-or-rule
    , {doStartAssign, 61 /* = */, 29, 101, TRUE}     //  99
    , {doNOP, 255, 37, 9, FALSE}     //  100
    , {doEndAssign, 59 /* ; */, 1,0,  TRUE}     //  101      assign-end
    , {doRuleErrorAssignExpr, 255, 103,0,  FALSE}     //  102
    , {doExit, 255, 103,0,  TRUE}     //  103      errorDeath
 };
#ifdef RBBI_DEBUG
static const char * const RBBIRuleStateNames[] = {    0,
     "start",
    0,
    0,
    0,
    0,
    0,
    0,
    0,
     "break-rule-end",
    0,
    0,
     "start-after-caret",
    0,
    0,
    0,
    0,
    0,
    0,
     "rev-option",
    0,
     "option-scan1",
    0,
     "option-scan2",
    0,
     "option-scan3",
    0,
    0,
     "reverse-rule",
     "term",
    0,
    0,
    0,
    0,
    0,
    0,
    0,
     "term-var-ref",
     "expr-mod",
    0,
    0,
    0,
    0,
     "expr-cont",
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
     "look-ahead",
    0,
     "expr-cont-no-slash",
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
     "tag-open",
    0,
    0,
     "tag-value",
    0,
    0,
    0,
     "tag-close",
    0,
    0,
     "expr-cont-no-tag",
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
     "scan-var-name",
    0,
     "scan-var-start",
    0,
     "scan-var-body",
    0,
     "scan-unicode-set",
    0,
    0,
    0,
     "assign-or-rule",
    0,
    0,
     "assign-end",
    0,
     "errorDeath",
    0};
#endif

U_NAMESPACE_END
#endif
