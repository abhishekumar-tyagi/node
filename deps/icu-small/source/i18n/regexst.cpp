// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
//
//  regexst.h
//
//  Copyright (C) 2004-2015, International Business Machines Corporation and others.
//  All Rights Reserved.
//
//  This file contains class RegexStaticSets
//
//  This class is internal to the regular expression implementation.
//  For the public Regular Expression API, see the file "unicode/regex.h"
//
//  RegexStaticSets groups together the common UnicodeSets that are needed
//   for compiling or executing RegularExpressions.  This grouping simplifies
//   the thread safe lazy creation and sharing of these sets across
//   all instances of regular expressions.
//
#include "unicode/utypes.h"

#if !UCONFIG_NO_REGULAR_EXPRESSIONS

#include "unicode/unistr.h"
#include "unicode/uniset.h"
#include "unicode/uchar.h"
#include "unicode/regex.h"
#include "uprops.h"
#include "cmemory.h"
#include "cstring.h"
#include "uassert.h"
#include "ucln_in.h"
#include "umutex.h"

#include "regexcst.h"   // Contains state table for the regex pattern parser.
                        //   generated by a Perl script.
#include "regexst.h"

U_NAMESPACE_BEGIN

// "Rule Char" Characters are those with special meaning, and therefore
//    need to be escaped to appear as literals in a regexp.
constexpr char16_t const *gRuleSet_rule_chars = u"*?+[(){}^$|\\.";

//
//   The backslash escape characters that ICU's unescape() function will handle.
//
constexpr char16_t const *gUnescapeChars = u"acefnrtuUx";

//
//  Unicode Set pattern for Regular Expression  \w
//
constexpr char16_t const *gIsWordPattern = u"[\\p{Alphabetic}\\p{M}\\p{Nd}\\p{Pc}\\u200c\\u200d]";

//
//  Unicode Set Definitions for Regular Expression  \s
//
constexpr  char16_t const *gIsSpacePattern = u"[\\p{WhiteSpace}]";

//
//  UnicodeSets used in implementation of Grapheme Cluster detection, \X
//
constexpr char16_t const *gGC_ControlPattern = u"[[:Zl:][:Zp:][:Cc:][:Cf:]-[:Grapheme_Extend:]]";
constexpr char16_t const *gGC_ExtendPattern  = u"[\\p{Grapheme_Extend}]";
constexpr char16_t const *gGC_LPattern       = u"[\\p{Hangul_Syllable_Type=L}]";
constexpr char16_t const *gGC_VPattern       = u"[\\p{Hangul_Syllable_Type=V}]";
constexpr char16_t const *gGC_TPattern       = u"[\\p{Hangul_Syllable_Type=T}]";
constexpr char16_t const *gGC_LVPattern      = u"[\\p{Hangul_Syllable_Type=LV}]";
constexpr char16_t const *gGC_LVTPattern     = u"[\\p{Hangul_Syllable_Type=LVT}]";


RegexStaticSets *RegexStaticSets::gStaticSets = nullptr;
UInitOnce gStaticSetsInitOnce = U_INITONCE_INITIALIZER;


RegexStaticSets::RegexStaticSets(UErrorCode *status) {
    // Initialize the shared static sets to their correct values.
    fUnescapeCharSet.addAll(UnicodeString(true, gUnescapeChars, -1)).freeze();
    fPropSets[URX_ISWORD_SET].applyPattern(UnicodeString(true, gIsWordPattern, -1), *status).freeze();
    fPropSets[URX_ISSPACE_SET].applyPattern(UnicodeString(true, gIsSpacePattern, -1), *status).freeze();
    fPropSets[URX_GC_EXTEND].applyPattern(UnicodeString(TRUE, gGC_ExtendPattern, -1), *status).freeze();
    fPropSets[URX_GC_CONTROL].applyPattern(UnicodeString(TRUE, gGC_ControlPattern, -1), *status).freeze();
    fPropSets[URX_GC_L].applyPattern(UnicodeString(TRUE, gGC_LPattern, -1), *status).freeze();
    fPropSets[URX_GC_V].applyPattern(UnicodeString(TRUE, gGC_VPattern, -1), *status).freeze();
    fPropSets[URX_GC_T].applyPattern(UnicodeString(TRUE, gGC_TPattern, -1), *status).freeze();
    fPropSets[URX_GC_LV].applyPattern(UnicodeString(TRUE, gGC_LVPattern, -1), *status).freeze();
    fPropSets[URX_GC_LVT].applyPattern(UnicodeString(TRUE, gGC_LVTPattern, -1), *status).freeze();


    //
    //  "Normal" is the set of characters that don't need special handling
    //            when finding grapheme cluster boundaries.
    //
    fPropSets[URX_GC_NORMAL].complement();
    fPropSets[URX_GC_NORMAL].remove(0xac00, 0xd7a4);
    fPropSets[URX_GC_NORMAL].removeAll(fPropSets[URX_GC_CONTROL]);
    fPropSets[URX_GC_NORMAL].removeAll(fPropSets[URX_GC_L]);
    fPropSets[URX_GC_NORMAL].removeAll(fPropSets[URX_GC_V]);
    fPropSets[URX_GC_NORMAL].removeAll(fPropSets[URX_GC_T]);
    fPropSets[URX_GC_NORMAL].freeze();

    // Initialize the 8-bit fast bit sets from the parallel full
    //   UnicodeSets.
    //
    // TODO: 25 Oct 2019 are these fast 8-bit sets worth keeping?
    //       Measured 3.5% gain on (non) matching with the pattern "x(?:\\S+)+x"
    //       This runs in exponential time, making it easy to adjust the time for
    //       convenient measuring.
    //
    //       This 8 bit optimization dates from the early days of ICU,
    //       with a less optimized UnicodeSet. At the time, the difference
    //       was substantial.

    for (int32_t i=0; i<URX_LAST_SET; i++) {
        fPropSets8[i].init(&fPropSets[i]);
    }

    // Sets used while parsing rules, but not referenced from the parse state table
    fRuleSets[kRuleSet_rule_char-128]
            .addAll(UnicodeString(gRuleSet_rule_chars)).complement().freeze();

    fRuleSets[kRuleSet_digit_char-128].add(u'0', u'9').freeze();
    fRuleSets[kRuleSet_ascii_letter-128].add(u'A', u'Z').add(u'a', u'z').freeze();
    fRuleDigitsAlias = &fRuleSets[kRuleSet_digit_char-128];

    // Finally, initialize an empty UText string for utility purposes
    fEmptyText = utext_openUChars(nullptr, nullptr, 0, status);

}


RegexStaticSets::~RegexStaticSets() {
    fRuleDigitsAlias = nullptr;
    utext_close(fEmptyText);
}


//------------------------------------------------------------------------------
//
//   regex_cleanup      Memory cleanup function, free/delete all
//                      cached memory.  Called by ICU's u_cleanup() function.
//
//------------------------------------------------------------------------------

U_CDECL_BEGIN
static UBool U_CALLCONV
regex_cleanup(void) {
    delete RegexStaticSets::gStaticSets;
    RegexStaticSets::gStaticSets = nullptr;
    gStaticSetsInitOnce.reset();
    return TRUE;
}

static void U_CALLCONV initStaticSets(UErrorCode &status) {
    U_ASSERT(RegexStaticSets::gStaticSets == nullptr);
    ucln_i18n_registerCleanup(UCLN_I18N_REGEX, regex_cleanup);
    RegexStaticSets::gStaticSets = new RegexStaticSets(&status);
    if (U_FAILURE(status)) {
        delete RegexStaticSets::gStaticSets;
        RegexStaticSets::gStaticSets = nullptr;
    }
    if (RegexStaticSets::gStaticSets == nullptr && U_SUCCESS(status)) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
}
U_CDECL_END

void RegexStaticSets::initGlobals(UErrorCode *status) {
    umtx_initOnce(gStaticSetsInitOnce, &initStaticSets, *status);
}

U_NAMESPACE_END
#endif  // !UCONFIG_NO_REGULAR_EXPRESSIONS
