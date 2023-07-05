// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2009-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  normalizer2.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2009nov22
*   created by: Markus W. Scherer
*/

#include "unicode/utypes.h"

#if !UCONFIG_NO_NORMALIZATION

#include "unicode/edits.h"
#include "unicode/normalizer2.h"
#include "unicode/stringoptions.h"
#include "unicode/unistr.h"
#include "unicode/unorm.h"
#include "cstring.h"
#include "mutex.h"
#include "norm2allmodes.h"
#include "normalizer2impl.h"
#include "uassert.h"
#include "ucln_cmn.h"

using icu::Normalizer2Impl;

#if NORM2_HARDCODE_NFC_DATA
// NFC/NFD data machine-generated by gennorm2 --csource
#define INCLUDED_FROM_NORMALIZER2_CPP
#include "norm2_nfc_data.h"
#endif

U_NAMESPACE_BEGIN

// Public API dispatch via Normalizer2 subclasses -------------------------- ***

Normalizer2::~Normalizer2() {}

void
Normalizer2::normalizeUTF8(uint32_t /*options*/, StringPiece src, ByteSink &sink,
                           Edits *edits, UErrorCode &errorCode) const {
    if (U_FAILURE(errorCode)) {
        return;
    }
    if (edits != nullptr) {
        errorCode = U_UNSUPPORTED_ERROR;
        return;
    }
    UnicodeString src16 = UnicodeString::fromUTF8(src);
    normalize(src16, errorCode).toUTF8(sink);
}

UBool
Normalizer2::getRawDecomposition(UChar32, UnicodeString &) const {
    return false;
}

UChar32
Normalizer2::composePair(UChar32, UChar32) const {
    return U_SENTINEL;
}

uint8_t
Normalizer2::getCombiningClass(UChar32 /*c*/) const {
    return 0;
}

UBool
Normalizer2::isNormalizedUTF8(StringPiece s, UErrorCode &errorCode) const {
    return U_SUCCESS(errorCode) && isNormalized(UnicodeString::fromUTF8(s), errorCode);
}

// Normalizer2 implementation for the old UNORM_NONE.
class NoopNormalizer2 : public Normalizer2 {
    virtual ~NoopNormalizer2();

    virtual UnicodeString &
    normalize(const UnicodeString &src,
              UnicodeString &dest,
              UErrorCode &errorCode) const override {
        if(U_SUCCESS(errorCode)) {
            if(&dest!=&src) {
                dest=src;
            } else {
                errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            }
        }
        return dest;
    }
    virtual void
    normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                  Edits *edits, UErrorCode &errorCode) const override {
        if(U_SUCCESS(errorCode)) {
            if (edits != nullptr) {
                if ((options & U_EDITS_NO_RESET) == 0) {
                    edits->reset();
                }
                edits->addUnchanged(src.length());
            }
            if ((options & U_OMIT_UNCHANGED_TEXT) == 0) {
                sink.Append(src.data(), src.length());
            }
            sink.Flush();
        }
    }

    virtual UnicodeString &
    normalizeSecondAndAppend(UnicodeString &first,
                             const UnicodeString &second,
                             UErrorCode &errorCode) const override {
        if(U_SUCCESS(errorCode)) {
            if(&first!=&second) {
                first.append(second);
            } else {
                errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            }
        }
        return first;
    }
    virtual UnicodeString &
    append(UnicodeString &first,
           const UnicodeString &second,
           UErrorCode &errorCode) const override {
        if(U_SUCCESS(errorCode)) {
            if(&first!=&second) {
                first.append(second);
            } else {
                errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            }
        }
        return first;
    }
    virtual UBool
    getDecomposition(UChar32, UnicodeString &) const override {
        return false;
    }
    // No need to override the default getRawDecomposition().
    virtual UBool
    isNormalized(const UnicodeString &, UErrorCode &errorCode) const override {
        return U_SUCCESS(errorCode);
    }
    virtual UBool
    isNormalizedUTF8(StringPiece, UErrorCode &errorCode) const override {
        return U_SUCCESS(errorCode);
    }
    virtual UNormalizationCheckResult
    quickCheck(const UnicodeString &, UErrorCode &) const override {
        return UNORM_YES;
    }
    virtual int32_t
    spanQuickCheckYes(const UnicodeString &s, UErrorCode &) const override {
        return s.length();
    }
    virtual UBool hasBoundaryBefore(UChar32) const override { return true; }
    virtual UBool hasBoundaryAfter(UChar32) const override { return true; }
    virtual UBool isInert(UChar32) const override { return true; }
};

NoopNormalizer2::~NoopNormalizer2() {}

Normalizer2WithImpl::~Normalizer2WithImpl() {}

DecomposeNormalizer2::~DecomposeNormalizer2() {}

ComposeNormalizer2::~ComposeNormalizer2() {}

FCDNormalizer2::~FCDNormalizer2() {}

// instance cache ---------------------------------------------------------- ***

U_CDECL_BEGIN
static UBool U_CALLCONV uprv_normalizer2_cleanup();
U_CDECL_END

static Normalizer2   *noopSingleton;
static icu::UInitOnce noopInitOnce {};

static void U_CALLCONV initNoopSingleton(UErrorCode &errorCode) {
    if(U_FAILURE(errorCode)) {
        return;
    }
    noopSingleton=new NoopNormalizer2;
    if(noopSingleton==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        return;
    }
    ucln_common_registerCleanup(UCLN_COMMON_NORMALIZER2, uprv_normalizer2_cleanup);
}

const Normalizer2 *Normalizer2Factory::getNoopInstance(UErrorCode &errorCode) {
    if(U_FAILURE(errorCode)) { return nullptr; }
    umtx_initOnce(noopInitOnce, &initNoopSingleton, errorCode);
    return noopSingleton;
}

const Normalizer2Impl *
Normalizer2Factory::getImpl(const Normalizer2 *norm2) {
    return &((Normalizer2WithImpl *)norm2)->impl;
}

Norm2AllModes::~Norm2AllModes() {
    delete impl;
}

Norm2AllModes *
Norm2AllModes::createInstance(Normalizer2Impl *impl, UErrorCode &errorCode) {
    if(U_FAILURE(errorCode)) {
        delete impl;
        return nullptr;
    }
    Norm2AllModes *allModes=new Norm2AllModes(impl);
    if(allModes==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        delete impl;
        return nullptr;
    }
    return allModes;
}

#if NORM2_HARDCODE_NFC_DATA
Norm2AllModes *
Norm2AllModes::createNFCInstance(UErrorCode &errorCode) {
    if(U_FAILURE(errorCode)) {
        return nullptr;
    }
    Normalizer2Impl *impl=new Normalizer2Impl;
    if(impl==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        return nullptr;
    }
    impl->init(norm2_nfc_data_indexes, &norm2_nfc_data_trie,
               norm2_nfc_data_extraData, norm2_nfc_data_smallFCD);
    return createInstance(impl, errorCode);
}

static Norm2AllModes *nfcSingleton;

static icu::UInitOnce nfcInitOnce {};

static void U_CALLCONV initNFCSingleton(UErrorCode &errorCode) {
    nfcSingleton=Norm2AllModes::createNFCInstance(errorCode);
    ucln_common_registerCleanup(UCLN_COMMON_NORMALIZER2, uprv_normalizer2_cleanup);
}

const Norm2AllModes *
Norm2AllModes::getNFCInstance(UErrorCode &errorCode) {
    if(U_FAILURE(errorCode)) { return nullptr; }
    umtx_initOnce(nfcInitOnce, &initNFCSingleton, errorCode);
    return nfcSingleton;
}

const Normalizer2 *
Normalizer2::getNFCInstance(UErrorCode &errorCode) {
    const Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    return allModes!=nullptr ? &allModes->comp : nullptr;
}

const Normalizer2 *
Normalizer2::getNFDInstance(UErrorCode &errorCode) {
    const Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    return allModes!=nullptr ? &allModes->decomp : nullptr;
}

const Normalizer2 *Normalizer2Factory::getFCDInstance(UErrorCode &errorCode) {
    const Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    return allModes!=nullptr ? &allModes->fcd : nullptr;
}

const Normalizer2 *Normalizer2Factory::getFCCInstance(UErrorCode &errorCode) {
    const Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    return allModes!=nullptr ? &allModes->fcc : nullptr;
}

const Normalizer2Impl *
Normalizer2Factory::getNFCImpl(UErrorCode &errorCode) {
    const Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    return allModes!=nullptr ? allModes->impl : nullptr;
}
#endif  // NORM2_HARDCODE_NFC_DATA

U_CDECL_BEGIN

static UBool U_CALLCONV uprv_normalizer2_cleanup() {
    delete noopSingleton;
    noopSingleton = nullptr;
    noopInitOnce.reset(); 
#if NORM2_HARDCODE_NFC_DATA
    delete nfcSingleton;
    nfcSingleton = nullptr;
    nfcInitOnce.reset(); 
#endif
    return true;
}

U_CDECL_END

U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI const UNormalizer2 * U_EXPORT2
unorm2_getNFCInstance(UErrorCode *pErrorCode) {
    return (const UNormalizer2 *)Normalizer2::getNFCInstance(*pErrorCode);
}

U_CAPI const UNormalizer2 * U_EXPORT2
unorm2_getNFDInstance(UErrorCode *pErrorCode) {
    return (const UNormalizer2 *)Normalizer2::getNFDInstance(*pErrorCode);
}

U_CAPI void U_EXPORT2
unorm2_close(UNormalizer2 *norm2) {
    delete (Normalizer2 *)norm2;
}

U_CAPI int32_t U_EXPORT2
unorm2_normalize(const UNormalizer2 *norm2,
                 const char16_t *src, int32_t length,
                 char16_t *dest, int32_t capacity,
                 UErrorCode *pErrorCode) {
    if(U_FAILURE(*pErrorCode)) {
        return 0;
    }
    if( (src==nullptr ? length!=0 : length<-1) ||
        (dest==nullptr ? capacity!=0 : capacity<0) ||
        (src==dest && src!=nullptr)
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        return 0;
    }
    UnicodeString destString(dest, 0, capacity);
    // length==0: Nothing to do, and n2wi->normalize(nullptr, nullptr, buffer, ...) would crash.
    if(length!=0) {
        const Normalizer2 *n2=(const Normalizer2 *)norm2;
        const Normalizer2WithImpl *n2wi=dynamic_cast<const Normalizer2WithImpl *>(n2);
        if(n2wi!=nullptr) {
            // Avoid duplicate argument checking and support NUL-terminated src.
            ReorderingBuffer buffer(n2wi->impl, destString);
            if(buffer.init(length, *pErrorCode)) {
                n2wi->normalize(src, length>=0 ? src+length : nullptr, buffer, *pErrorCode);
            }
        } else {
            UnicodeString srcString(length<0, src, length);
            n2->normalize(srcString, destString, *pErrorCode);
        }
    }
    return destString.extract(dest, capacity, *pErrorCode);
}

static int32_t
normalizeSecondAndAppend(const UNormalizer2 *norm2,
                         char16_t *first, int32_t firstLength, int32_t firstCapacity,
                         const char16_t *second, int32_t secondLength,
                         UBool doNormalize,
                         UErrorCode *pErrorCode) {
    if(U_FAILURE(*pErrorCode)) {
        return 0;
    }
    if( (second==nullptr ? secondLength!=0 : secondLength<-1) ||
        (first==nullptr ? (firstCapacity!=0 || firstLength!=0) :
                       (firstCapacity<0 || firstLength<-1)) ||
        (first==second && first!=nullptr)
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        return 0;
    }
    UnicodeString firstString(first, firstLength, firstCapacity);
    firstLength=firstString.length();  // In case it was -1.
    // secondLength==0: Nothing to do, and n2wi->normalizeAndAppend(nullptr, nullptr, buffer, ...) would crash.
    if(secondLength!=0) {
        const Normalizer2 *n2=(const Normalizer2 *)norm2;
        const Normalizer2WithImpl *n2wi=dynamic_cast<const Normalizer2WithImpl *>(n2);
        if(n2wi!=nullptr) {
            // Avoid duplicate argument checking and support NUL-terminated src.
            UnicodeString safeMiddle;
            {
                ReorderingBuffer buffer(n2wi->impl, firstString);
                if(buffer.init(firstLength+secondLength+1, *pErrorCode)) {  // destCapacity>=-1
                    n2wi->normalizeAndAppend(second, secondLength>=0 ? second+secondLength : nullptr,
                                             doNormalize, safeMiddle, buffer, *pErrorCode);
                }
            }  // The ReorderingBuffer destructor finalizes firstString.
            if(U_FAILURE(*pErrorCode) || firstString.length()>firstCapacity) {
                // Restore the modified suffix of the first string.
                // This does not restore first[] array contents between firstLength and firstCapacity.
                // (That might be uninitialized memory, as far as we know.)
                if(first!=nullptr) { /* don't dereference nullptr */
                  safeMiddle.extract(0, 0x7fffffff, first+firstLength-safeMiddle.length());
                  if(firstLength<firstCapacity) {
                    first[firstLength]=0;  // NUL-terminate in case it was originally.
                  }
                }
            }
        } else {
            UnicodeString secondString(secondLength<0, second, secondLength);
            if(doNormalize) {
                n2->normalizeSecondAndAppend(firstString, secondString, *pErrorCode);
            } else {
                n2->append(firstString, secondString, *pErrorCode);
            }
        }
    }
    return firstString.extract(first, firstCapacity, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
unorm2_normalizeSecondAndAppend(const UNormalizer2 *norm2,
                                char16_t *first, int32_t firstLength, int32_t firstCapacity,
                                const char16_t *second, int32_t secondLength,
                                UErrorCode *pErrorCode) {
    return normalizeSecondAndAppend(norm2,
                                    first, firstLength, firstCapacity,
                                    second, secondLength,
                                    true, pErrorCode);
}

U_CAPI int32_t U_EXPORT2
unorm2_append(const UNormalizer2 *norm2,
              char16_t *first, int32_t firstLength, int32_t firstCapacity,
              const char16_t *second, int32_t secondLength,
              UErrorCode *pErrorCode) {
    return normalizeSecondAndAppend(norm2,
                                    first, firstLength, firstCapacity,
                                    second, secondLength,
                                    false, pErrorCode);
}

U_CAPI int32_t U_EXPORT2
unorm2_getDecomposition(const UNormalizer2 *norm2,
                        UChar32 c, char16_t *decomposition, int32_t capacity,
                        UErrorCode *pErrorCode) {
    if(U_FAILURE(*pErrorCode)) {
        return 0;
    }
    if(decomposition==nullptr ? capacity!=0 : capacity<0) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        return 0;
    }
    UnicodeString destString(decomposition, 0, capacity);
    if(reinterpret_cast<const Normalizer2 *>(norm2)->getDecomposition(c, destString)) {
        return destString.extract(decomposition, capacity, *pErrorCode);
    } else {
        return -1;
    }
}

U_CAPI int32_t U_EXPORT2
unorm2_getRawDecomposition(const UNormalizer2 *norm2,
                           UChar32 c, char16_t *decomposition, int32_t capacity,
                           UErrorCode *pErrorCode) {
    if(U_FAILURE(*pErrorCode)) {
        return 0;
    }
    if(decomposition==nullptr ? capacity!=0 : capacity<0) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        return 0;
    }
    UnicodeString destString(decomposition, 0, capacity);
    if(reinterpret_cast<const Normalizer2 *>(norm2)->getRawDecomposition(c, destString)) {
        return destString.extract(decomposition, capacity, *pErrorCode);
    } else {
        return -1;
    }
}

U_CAPI UChar32 U_EXPORT2
unorm2_composePair(const UNormalizer2 *norm2, UChar32 a, UChar32 b) {
    return reinterpret_cast<const Normalizer2 *>(norm2)->composePair(a, b);
}

U_CAPI uint8_t U_EXPORT2
unorm2_getCombiningClass(const UNormalizer2 *norm2, UChar32 c) {
    return reinterpret_cast<const Normalizer2 *>(norm2)->getCombiningClass(c);
}

U_CAPI UBool U_EXPORT2
unorm2_isNormalized(const UNormalizer2 *norm2,
                    const char16_t *s, int32_t length,
                    UErrorCode *pErrorCode) {
    if(U_FAILURE(*pErrorCode)) {
        return 0;
    }
    if((s==nullptr && length!=0) || length<-1) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        return 0;
    }
    UnicodeString sString(length<0, s, length);
    return ((const Normalizer2 *)norm2)->isNormalized(sString, *pErrorCode);
}

U_CAPI UNormalizationCheckResult U_EXPORT2
unorm2_quickCheck(const UNormalizer2 *norm2,
                  const char16_t *s, int32_t length,
                  UErrorCode *pErrorCode) {
    if(U_FAILURE(*pErrorCode)) {
        return UNORM_NO;
    }
    if((s==nullptr && length!=0) || length<-1) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        return UNORM_NO;
    }
    UnicodeString sString(length<0, s, length);
    return ((const Normalizer2 *)norm2)->quickCheck(sString, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
unorm2_spanQuickCheckYes(const UNormalizer2 *norm2,
                         const char16_t *s, int32_t length,
                         UErrorCode *pErrorCode) {
    if(U_FAILURE(*pErrorCode)) {
        return 0;
    }
    if((s==nullptr && length!=0) || length<-1) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        return 0;
    }
    UnicodeString sString(length<0, s, length);
    return ((const Normalizer2 *)norm2)->spanQuickCheckYes(sString, *pErrorCode);
}

U_CAPI UBool U_EXPORT2
unorm2_hasBoundaryBefore(const UNormalizer2 *norm2, UChar32 c) {
    return ((const Normalizer2 *)norm2)->hasBoundaryBefore(c);
}

U_CAPI UBool U_EXPORT2
unorm2_hasBoundaryAfter(const UNormalizer2 *norm2, UChar32 c) {
    return ((const Normalizer2 *)norm2)->hasBoundaryAfter(c);
}

U_CAPI UBool U_EXPORT2
unorm2_isInert(const UNormalizer2 *norm2, UChar32 c) {
    return ((const Normalizer2 *)norm2)->isInert(c);
}

// Some properties APIs ---------------------------------------------------- ***

U_CAPI uint8_t U_EXPORT2
u_getCombiningClass(UChar32 c) {
    UErrorCode errorCode=U_ZERO_ERROR;
    const Normalizer2 *nfd=Normalizer2::getNFDInstance(errorCode);
    if(U_SUCCESS(errorCode)) {
        return nfd->getCombiningClass(c);
    } else {
        return 0;
    }
}

U_CFUNC uint16_t
unorm_getFCD16(UChar32 c) {
    UErrorCode errorCode=U_ZERO_ERROR;
    const Normalizer2Impl *impl=Normalizer2Factory::getNFCImpl(errorCode);
    if(U_SUCCESS(errorCode)) {
        return impl->getFCD16(c);
    } else {
        return 0;
    }
}

#endif  // !UCONFIG_NO_NORMALIZATION
