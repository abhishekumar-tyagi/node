// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
//---------------------------------------------------------------------------------
//
// Generated Header File.  Do not edit by hand.
//    This file contains the state table for the ICU Regular Expression Pattern Parser
//    It is generated by the Perl script "regexcst.pl" from
//    the rule parser state definitions file "regexcst.txt".
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
// Character classes for regex pattern scanning.
//
    static const uint8_t kRuleSet_digit_char = 128;
    static const uint8_t kRuleSet_ascii_letter = 129;
    static const uint8_t kRuleSet_rule_char = 130;
    constexpr uint32_t kRuleSet_count = 131-128;

enum Regex_PatternParseAction {
    doSetBackslash_D,
    doBackslashh,
    doBackslashH,
    doSetLiteralEscaped,
    doOpenLookAheadNeg,
    doCompleteNamedBackRef,
    doPatStart,
    doBackslashS,
    doBackslashD,
    doNGStar,
    doNOP,
    doBackslashX,
    doSetLiteral,
    doContinueNamedCapture,
    doBackslashG,
    doBackslashR,
    doSetBegin,
    doSetBackslash_v,
    doPossessivePlus,
    doPerlInline,
    doBackslashZ,
    doSetAddAmp,
    doSetBeginDifference1,
    doIntervalError,
    doSetNegate,
    doIntervalInit,
    doSetIntersection2,
    doPossessiveInterval,
    doRuleError,
    doBackslashW,
    doContinueNamedBackRef,
    doOpenNonCaptureParen,
    doExit,
    doSetNamedChar,
    doSetBackslash_V,
    doConditionalExpr,
    doEscapeError,
    doBadOpenParenType,
    doPossessiveStar,
    doSetAddDash,
    doEscapedLiteralChar,
    doSetBackslash_w,
    doIntervalUpperDigit,
    doBackslashv,
    doSetBackslash_S,
    doSetNoCloseError,
    doSetProp,
    doBackslashB,
    doSetEnd,
    doSetRange,
    doMatchModeParen,
    doPlus,
    doBackslashV,
    doSetMatchMode,
    doBackslashz,
    doSetNamedRange,
    doOpenLookBehindNeg,
    doInterval,
    doBadNamedCapture,
    doBeginMatchMode,
    doBackslashd,
    doPatFinish,
    doNamedChar,
    doNGPlus,
    doSetDifference2,
    doSetBackslash_H,
    doCloseParen,
    doDotAny,
    doOpenCaptureParen,
    doEnterQuoteMode,
    doOpenAtomicParen,
    doBadModeFlag,
    doSetBackslash_d,
    doSetFinish,
    doProperty,
    doBeginNamedBackRef,
    doBackRef,
    doOpt,
    doDollar,
    doBeginNamedCapture,
    doNGInterval,
    doSetOpError,
    doSetPosixProp,
    doSetBeginIntersection1,
    doBackslashb,
    doSetBeginUnion,
    doIntevalLowerDigit,
    doSetBackslash_h,
    doStar,
    doMatchMode,
    doBackslashA,
    doOpenLookBehind,
    doPossessiveOpt,
    doOrOperator,
    doBackslashw,
    doBackslashs,
    doLiteralChar,
    doSuppressComments,
    doCaret,
    doIntervalSame,
    doNGOpt,
    doOpenLookAhead,
    doSetBackslash_W,
    doMismatchedParenErr,
    doSetBackslash_s,
    rbbiLastAction};

//-------------------------------------------------------------------------------
//
//  RegexTableEl       represents the structure of a row in the transition table
//                     for the pattern parser state machine.
//-------------------------------------------------------------------------------
struct RegexTableEl {
    Regex_PatternParseAction      fAction;
    uint8_t                       fCharClass;       // 0-127:    an individual ASCII character
                                                    // 128-255:  character class index
    uint8_t                       fNextState;       // 0-250:    normal next-state numbers
                                                    // 255:      pop next-state from stack.
    uint8_t                       fPushState;
    UBool                         fNextChar;
};

static const struct RegexTableEl gRuleParseStateTable[] = {
    {doNOP, 0, 0, 0, TRUE}
    , {doPatStart, 255, 2,0,  FALSE}     //  1      start
    , {doLiteralChar, 254, 14,0,  TRUE}     //  2      term
    , {doLiteralChar, 130, 14,0,  TRUE}     //  3 
    , {doSetBegin, 91 /* [ */, 123, 205, TRUE}     //  4 
    , {doNOP, 40 /* ( */, 27,0,  TRUE}     //  5 
    , {doDotAny, 46 /* . */, 14,0,  TRUE}     //  6 
    , {doCaret, 94 /* ^ */, 14,0,  TRUE}     //  7 
    , {doDollar, 36 /* $ */, 14,0,  TRUE}     //  8 
    , {doNOP, 92 /* \ */, 89,0,  TRUE}     //  9 
    , {doOrOperator, 124 /* | */, 2,0,  TRUE}     //  10 
    , {doCloseParen, 41 /* ) */, 255,0,  TRUE}     //  11 
    , {doPatFinish, 253, 2,0,  FALSE}     //  12 
    , {doRuleError, 255, 206,0,  FALSE}     //  13 
    , {doNOP, 42 /* * */, 68,0,  TRUE}     //  14      expr-quant
    , {doNOP, 43 /* + */, 71,0,  TRUE}     //  15 
    , {doNOP, 63 /* ? */, 74,0,  TRUE}     //  16 
    , {doIntervalInit, 123 /* { */, 77,0,  TRUE}     //  17 
    , {doNOP, 40 /* ( */, 23,0,  TRUE}     //  18 
    , {doNOP, 255, 20,0,  FALSE}     //  19 
    , {doOrOperator, 124 /* | */, 2,0,  TRUE}     //  20      expr-cont
    , {doCloseParen, 41 /* ) */, 255,0,  TRUE}     //  21 
    , {doNOP, 255, 2,0,  FALSE}     //  22 
    , {doSuppressComments, 63 /* ? */, 25,0,  TRUE}     //  23      open-paren-quant
    , {doNOP, 255, 27,0,  FALSE}     //  24 
    , {doNOP, 35 /* # */, 50, 14, TRUE}     //  25      open-paren-quant2
    , {doNOP, 255, 29,0,  FALSE}     //  26 
    , {doSuppressComments, 63 /* ? */, 29,0,  TRUE}     //  27      open-paren
    , {doOpenCaptureParen, 255, 2, 14, FALSE}     //  28 
    , {doOpenNonCaptureParen, 58 /* : */, 2, 14, TRUE}     //  29      open-paren-extended
    , {doOpenAtomicParen, 62 /* > */, 2, 14, TRUE}     //  30 
    , {doOpenLookAhead, 61 /* = */, 2, 20, TRUE}     //  31 
    , {doOpenLookAheadNeg, 33 /* ! */, 2, 20, TRUE}     //  32 
    , {doNOP, 60 /* < */, 46,0,  TRUE}     //  33 
    , {doNOP, 35 /* # */, 50, 2, TRUE}     //  34 
    , {doBeginMatchMode, 105 /* i */, 53,0,  FALSE}     //  35 
    , {doBeginMatchMode, 100 /* d */, 53,0,  FALSE}     //  36 
    , {doBeginMatchMode, 109 /* m */, 53,0,  FALSE}     //  37 
    , {doBeginMatchMode, 115 /* s */, 53,0,  FALSE}     //  38 
    , {doBeginMatchMode, 117 /* u */, 53,0,  FALSE}     //  39 
    , {doBeginMatchMode, 119 /* w */, 53,0,  FALSE}     //  40 
    , {doBeginMatchMode, 120 /* x */, 53,0,  FALSE}     //  41 
    , {doBeginMatchMode, 45 /* - */, 53,0,  FALSE}     //  42 
    , {doConditionalExpr, 40 /* ( */, 206,0,  TRUE}     //  43 
    , {doPerlInline, 123 /* { */, 206,0,  TRUE}     //  44 
    , {doBadOpenParenType, 255, 206,0,  FALSE}     //  45 
    , {doOpenLookBehind, 61 /* = */, 2, 20, TRUE}     //  46      open-paren-lookbehind
    , {doOpenLookBehindNeg, 33 /* ! */, 2, 20, TRUE}     //  47 
    , {doBeginNamedCapture, 129, 64,0,  FALSE}     //  48 
    , {doBadOpenParenType, 255, 206,0,  FALSE}     //  49 
    , {doNOP, 41 /* ) */, 255,0,  TRUE}     //  50      paren-comment
    , {doMismatchedParenErr, 253, 206,0,  FALSE}     //  51 
    , {doNOP, 255, 50,0,  TRUE}     //  52 
    , {doMatchMode, 105 /* i */, 53,0,  TRUE}     //  53      paren-flag
    , {doMatchMode, 100 /* d */, 53,0,  TRUE}     //  54 
    , {doMatchMode, 109 /* m */, 53,0,  TRUE}     //  55 
    , {doMatchMode, 115 /* s */, 53,0,  TRUE}     //  56 
    , {doMatchMode, 117 /* u */, 53,0,  TRUE}     //  57 
    , {doMatchMode, 119 /* w */, 53,0,  TRUE}     //  58 
    , {doMatchMode, 120 /* x */, 53,0,  TRUE}     //  59 
    , {doMatchMode, 45 /* - */, 53,0,  TRUE}     //  60 
    , {doSetMatchMode, 41 /* ) */, 2,0,  TRUE}     //  61 
    , {doMatchModeParen, 58 /* : */, 2, 14, TRUE}     //  62 
    , {doBadModeFlag, 255, 206,0,  FALSE}     //  63 
    , {doContinueNamedCapture, 129, 64,0,  TRUE}     //  64      named-capture
    , {doContinueNamedCapture, 128, 64,0,  TRUE}     //  65 
    , {doOpenCaptureParen, 62 /* > */, 2, 14, TRUE}     //  66 
    , {doBadNamedCapture, 255, 206,0,  FALSE}     //  67 
    , {doNGStar, 63 /* ? */, 20,0,  TRUE}     //  68      quant-star
    , {doPossessiveStar, 43 /* + */, 20,0,  TRUE}     //  69 
    , {doStar, 255, 20,0,  FALSE}     //  70 
    , {doNGPlus, 63 /* ? */, 20,0,  TRUE}     //  71      quant-plus
    , {doPossessivePlus, 43 /* + */, 20,0,  TRUE}     //  72 
    , {doPlus, 255, 20,0,  FALSE}     //  73 
    , {doNGOpt, 63 /* ? */, 20,0,  TRUE}     //  74      quant-opt
    , {doPossessiveOpt, 43 /* + */, 20,0,  TRUE}     //  75 
    , {doOpt, 255, 20,0,  FALSE}     //  76 
    , {doNOP, 128, 79,0,  FALSE}     //  77      interval-open
    , {doIntervalError, 255, 206,0,  FALSE}     //  78 
    , {doIntevalLowerDigit, 128, 79,0,  TRUE}     //  79      interval-lower
    , {doNOP, 44 /* , */, 83,0,  TRUE}     //  80 
    , {doIntervalSame, 125 /* } */, 86,0,  TRUE}     //  81 
    , {doIntervalError, 255, 206,0,  FALSE}     //  82 
    , {doIntervalUpperDigit, 128, 83,0,  TRUE}     //  83      interval-upper
    , {doNOP, 125 /* } */, 86,0,  TRUE}     //  84 
    , {doIntervalError, 255, 206,0,  FALSE}     //  85 
    , {doNGInterval, 63 /* ? */, 20,0,  TRUE}     //  86      interval-type
    , {doPossessiveInterval, 43 /* + */, 20,0,  TRUE}     //  87 
    , {doInterval, 255, 20,0,  FALSE}     //  88 
    , {doBackslashA, 65 /* A */, 2,0,  TRUE}     //  89      backslash
    , {doBackslashB, 66 /* B */, 2,0,  TRUE}     //  90 
    , {doBackslashb, 98 /* b */, 2,0,  TRUE}     //  91 
    , {doBackslashd, 100 /* d */, 14,0,  TRUE}     //  92 
    , {doBackslashD, 68 /* D */, 14,0,  TRUE}     //  93 
    , {doBackslashG, 71 /* G */, 2,0,  TRUE}     //  94 
    , {doBackslashh, 104 /* h */, 14,0,  TRUE}     //  95 
    , {doBackslashH, 72 /* H */, 14,0,  TRUE}     //  96 
    , {doNOP, 107 /* k */, 115,0,  TRUE}     //  97 
    , {doNamedChar, 78 /* N */, 14,0,  FALSE}     //  98 
    , {doProperty, 112 /* p */, 14,0,  FALSE}     //  99 
    , {doProperty, 80 /* P */, 14,0,  FALSE}     //  100 
    , {doBackslashR, 82 /* R */, 14,0,  TRUE}     //  101 
    , {doEnterQuoteMode, 81 /* Q */, 2,0,  TRUE}     //  102 
    , {doBackslashS, 83 /* S */, 14,0,  TRUE}     //  103 
    , {doBackslashs, 115 /* s */, 14,0,  TRUE}     //  104 
    , {doBackslashv, 118 /* v */, 14,0,  TRUE}     //  105 
    , {doBackslashV, 86 /* V */, 14,0,  TRUE}     //  106 
    , {doBackslashW, 87 /* W */, 14,0,  TRUE}     //  107 
    , {doBackslashw, 119 /* w */, 14,0,  TRUE}     //  108 
    , {doBackslashX, 88 /* X */, 14,0,  TRUE}     //  109 
    , {doBackslashZ, 90 /* Z */, 2,0,  TRUE}     //  110 
    , {doBackslashz, 122 /* z */, 2,0,  TRUE}     //  111 
    , {doBackRef, 128, 14,0,  TRUE}     //  112 
    , {doEscapeError, 253, 206,0,  FALSE}     //  113 
    , {doEscapedLiteralChar, 255, 14,0,  TRUE}     //  114 
    , {doBeginNamedBackRef, 60 /* < */, 117,0,  TRUE}     //  115      named-backref
    , {doBadNamedCapture, 255, 206,0,  FALSE}     //  116 
    , {doContinueNamedBackRef, 129, 119,0,  TRUE}     //  117      named-backref-2
    , {doBadNamedCapture, 255, 206,0,  FALSE}     //  118 
    , {doContinueNamedBackRef, 129, 119,0,  TRUE}     //  119      named-backref-3
    , {doContinueNamedBackRef, 128, 119,0,  TRUE}     //  120 
    , {doCompleteNamedBackRef, 62 /* > */, 14,0,  TRUE}     //  121 
    , {doBadNamedCapture, 255, 206,0,  FALSE}     //  122 
    , {doSetNegate, 94 /* ^ */, 126,0,  TRUE}     //  123      set-open
    , {doSetPosixProp, 58 /* : */, 128,0,  FALSE}     //  124 
    , {doNOP, 255, 126,0,  FALSE}     //  125 
    , {doSetLiteral, 93 /* ] */, 141,0,  TRUE}     //  126      set-open2
    , {doNOP, 255, 131,0,  FALSE}     //  127 
    , {doSetEnd, 93 /* ] */, 255,0,  TRUE}     //  128      set-posix
    , {doNOP, 58 /* : */, 131,0,  FALSE}     //  129 
    , {doRuleError, 255, 206,0,  FALSE}     //  130 
    , {doSetEnd, 93 /* ] */, 255,0,  TRUE}     //  131      set-start
    , {doSetBeginUnion, 91 /* [ */, 123, 148, TRUE}     //  132 
    , {doNOP, 92 /* \ */, 191,0,  TRUE}     //  133 
    , {doNOP, 45 /* - */, 137,0,  TRUE}     //  134 
    , {doNOP, 38 /* & */, 139,0,  TRUE}     //  135 
    , {doSetLiteral, 255, 141,0,  TRUE}     //  136 
    , {doRuleError, 45 /* - */, 206,0,  FALSE}     //  137      set-start-dash
    , {doSetAddDash, 255, 141,0,  FALSE}     //  138 
    , {doRuleError, 38 /* & */, 206,0,  FALSE}     //  139      set-start-amp
    , {doSetAddAmp, 255, 141,0,  FALSE}     //  140 
    , {doSetEnd, 93 /* ] */, 255,0,  TRUE}     //  141      set-after-lit
    , {doSetBeginUnion, 91 /* [ */, 123, 148, TRUE}     //  142 
    , {doNOP, 45 /* - */, 178,0,  TRUE}     //  143 
    , {doNOP, 38 /* & */, 169,0,  TRUE}     //  144 
    , {doNOP, 92 /* \ */, 191,0,  TRUE}     //  145 
    , {doSetNoCloseError, 253, 206,0,  FALSE}     //  146 
    , {doSetLiteral, 255, 141,0,  TRUE}     //  147 
    , {doSetEnd, 93 /* ] */, 255,0,  TRUE}     //  148      set-after-set
    , {doSetBeginUnion, 91 /* [ */, 123, 148, TRUE}     //  149 
    , {doNOP, 45 /* - */, 171,0,  TRUE}     //  150 
    , {doNOP, 38 /* & */, 166,0,  TRUE}     //  151 
    , {doNOP, 92 /* \ */, 191,0,  TRUE}     //  152 
    , {doSetNoCloseError, 253, 206,0,  FALSE}     //  153 
    , {doSetLiteral, 255, 141,0,  TRUE}     //  154 
    , {doSetEnd, 93 /* ] */, 255,0,  TRUE}     //  155      set-after-range
    , {doSetBeginUnion, 91 /* [ */, 123, 148, TRUE}     //  156 
    , {doNOP, 45 /* - */, 174,0,  TRUE}     //  157 
    , {doNOP, 38 /* & */, 176,0,  TRUE}     //  158 
    , {doNOP, 92 /* \ */, 191,0,  TRUE}     //  159 
    , {doSetNoCloseError, 253, 206,0,  FALSE}     //  160 
    , {doSetLiteral, 255, 141,0,  TRUE}     //  161 
    , {doSetBeginUnion, 91 /* [ */, 123, 148, TRUE}     //  162      set-after-op
    , {doSetOpError, 93 /* ] */, 206,0,  FALSE}     //  163 
    , {doNOP, 92 /* \ */, 191,0,  TRUE}     //  164 
    , {doSetLiteral, 255, 141,0,  TRUE}     //  165 
    , {doSetBeginIntersection1, 91 /* [ */, 123, 148, TRUE}     //  166      set-set-amp
    , {doSetIntersection2, 38 /* & */, 162,0,  TRUE}     //  167 
    , {doSetAddAmp, 255, 141,0,  FALSE}     //  168 
    , {doSetIntersection2, 38 /* & */, 162,0,  TRUE}     //  169      set-lit-amp
    , {doSetAddAmp, 255, 141,0,  FALSE}     //  170 
    , {doSetBeginDifference1, 91 /* [ */, 123, 148, TRUE}     //  171      set-set-dash
    , {doSetDifference2, 45 /* - */, 162,0,  TRUE}     //  172 
    , {doSetAddDash, 255, 141,0,  FALSE}     //  173 
    , {doSetDifference2, 45 /* - */, 162,0,  TRUE}     //  174      set-range-dash
    , {doSetAddDash, 255, 141,0,  FALSE}     //  175 
    , {doSetIntersection2, 38 /* & */, 162,0,  TRUE}     //  176      set-range-amp
    , {doSetAddAmp, 255, 141,0,  FALSE}     //  177 
    , {doSetDifference2, 45 /* - */, 162,0,  TRUE}     //  178      set-lit-dash
    , {doSetAddDash, 91 /* [ */, 141,0,  FALSE}     //  179 
    , {doSetAddDash, 93 /* ] */, 141,0,  FALSE}     //  180 
    , {doNOP, 92 /* \ */, 183,0,  TRUE}     //  181 
    , {doSetRange, 255, 155,0,  TRUE}     //  182 
    , {doSetOpError, 115 /* s */, 206,0,  FALSE}     //  183      set-lit-dash-escape
    , {doSetOpError, 83 /* S */, 206,0,  FALSE}     //  184 
    , {doSetOpError, 119 /* w */, 206,0,  FALSE}     //  185 
    , {doSetOpError, 87 /* W */, 206,0,  FALSE}     //  186 
    , {doSetOpError, 100 /* d */, 206,0,  FALSE}     //  187 
    , {doSetOpError, 68 /* D */, 206,0,  FALSE}     //  188 
    , {doSetNamedRange, 78 /* N */, 155,0,  FALSE}     //  189 
    , {doSetRange, 255, 155,0,  TRUE}     //  190 
    , {doSetProp, 112 /* p */, 148,0,  FALSE}     //  191      set-escape
    , {doSetProp, 80 /* P */, 148,0,  FALSE}     //  192 
    , {doSetNamedChar, 78 /* N */, 141,0,  FALSE}     //  193 
    , {doSetBackslash_s, 115 /* s */, 155,0,  TRUE}     //  194 
    , {doSetBackslash_S, 83 /* S */, 155,0,  TRUE}     //  195 
    , {doSetBackslash_w, 119 /* w */, 155,0,  TRUE}     //  196 
    , {doSetBackslash_W, 87 /* W */, 155,0,  TRUE}     //  197 
    , {doSetBackslash_d, 100 /* d */, 155,0,  TRUE}     //  198 
    , {doSetBackslash_D, 68 /* D */, 155,0,  TRUE}     //  199 
    , {doSetBackslash_h, 104 /* h */, 155,0,  TRUE}     //  200 
    , {doSetBackslash_H, 72 /* H */, 155,0,  TRUE}     //  201 
    , {doSetBackslash_v, 118 /* v */, 155,0,  TRUE}     //  202 
    , {doSetBackslash_V, 86 /* V */, 155,0,  TRUE}     //  203 
    , {doSetLiteralEscaped, 255, 141,0,  TRUE}     //  204 
    , {doSetFinish, 255, 14,0,  FALSE}     //  205      set-finish
    , {doExit, 255, 206,0,  TRUE}     //  206      errorDeath
 };
static const char * const RegexStateNames[] = {    0,
     "start",
     "term",
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
     "expr-quant",
    0,
    0,
    0,
    0,
    0,
     "expr-cont",
    0,
    0,
     "open-paren-quant",
    0,
     "open-paren-quant2",
    0,
     "open-paren",
    0,
     "open-paren-extended",
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
    0,
    0,
    0,
    0,
    0,
     "open-paren-lookbehind",
    0,
    0,
    0,
     "paren-comment",
    0,
    0,
     "paren-flag",
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
     "named-capture",
    0,
    0,
    0,
     "quant-star",
    0,
    0,
     "quant-plus",
    0,
    0,
     "quant-opt",
    0,
    0,
     "interval-open",
    0,
     "interval-lower",
    0,
    0,
    0,
     "interval-upper",
    0,
    0,
     "interval-type",
    0,
    0,
     "backslash",
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
    0,
    0,
    0,
     "named-backref",
    0,
     "named-backref-2",
    0,
     "named-backref-3",
    0,
    0,
    0,
     "set-open",
    0,
    0,
     "set-open2",
    0,
     "set-posix",
    0,
    0,
     "set-start",
    0,
    0,
    0,
    0,
    0,
     "set-start-dash",
    0,
     "set-start-amp",
    0,
     "set-after-lit",
    0,
    0,
    0,
    0,
    0,
    0,
     "set-after-set",
    0,
    0,
    0,
    0,
    0,
    0,
     "set-after-range",
    0,
    0,
    0,
    0,
    0,
    0,
     "set-after-op",
    0,
    0,
    0,
     "set-set-amp",
    0,
    0,
     "set-lit-amp",
    0,
     "set-set-dash",
    0,
    0,
     "set-range-dash",
    0,
     "set-range-amp",
    0,
     "set-lit-dash",
    0,
    0,
    0,
    0,
     "set-lit-dash-escape",
    0,
    0,
    0,
    0,
    0,
    0,
    0,
     "set-escape",
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
    0,
    0,
     "set-finish",
     "errorDeath",
    0};

U_NAMESPACE_END
#endif
