// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#include "unicode/utypes.h"

#if !UCONFIG_NO_FORMATTING
#ifndef __NUMBER_MAPPER_H__
#define __NUMBER_MAPPER_H__

#include <atomic>
#include "number_types.h"
#include "unicode/currpinf.h"
#include "standardplural.h"
#include "number_patternstring.h"
#include "number_currencysymbols.h"
#include "numparse_impl.h"

U_NAMESPACE_BEGIN
namespace number {
namespace impl {


class PropertiesAffixPatternProvider : public AffixPatternProvider, public UMemory {
  public:
    bool isBogus() const {
        return fBogus;
    }

    void setToBogus() {
        fBogus = true;
    }

    void setTo(const DecimalFormatProperties& properties, UErrorCode& status);

    PropertiesAffixPatternProvider() = default; // puts instance in valid but undefined state

    PropertiesAffixPatternProvider(const DecimalFormatProperties& properties, UErrorCode& status) {
        setTo(properties, status);
    }

    // AffixPatternProvider Methods:

    char16_t charAt(int32_t flags, int32_t i) const U_OVERRIDE;

    int32_t length(int32_t flags) const U_OVERRIDE;

    UnicodeString getString(int32_t flags) const U_OVERRIDE;

    bool hasCurrencySign() const U_OVERRIDE;

    bool positiveHasPlusSign() const U_OVERRIDE;

    bool hasNegativeSubpattern() const U_OVERRIDE;

    bool negativeHasMinusSign() const U_OVERRIDE;

    bool containsSymbolType(AffixPatternType, UErrorCode&) const U_OVERRIDE;

    bool hasBody() const U_OVERRIDE;

  private:
    UnicodeString posPrefix;
    UnicodeString posSuffix;
    UnicodeString negPrefix;
    UnicodeString negSuffix;
    bool isCurrencyPattern;

    const UnicodeString& getStringInternal(int32_t flags) const;

    bool fBogus{true};
};


class CurrencyPluralInfoAffixProvider : public AffixPatternProvider, public UMemory {
  public:
    bool isBogus() const {
        return fBogus;
    }

    void setToBogus() {
        fBogus = true;
    }

    void setTo(const CurrencyPluralInfo& cpi, const DecimalFormatProperties& properties,
               UErrorCode& status);

    // AffixPatternProvider Methods:

    char16_t charAt(int32_t flags, int32_t i) const U_OVERRIDE;

    int32_t length(int32_t flags) const U_OVERRIDE;

    UnicodeString getString(int32_t flags) const U_OVERRIDE;

    bool hasCurrencySign() const U_OVERRIDE;

    bool positiveHasPlusSign() const U_OVERRIDE;

    bool hasNegativeSubpattern() const U_OVERRIDE;

    bool negativeHasMinusSign() const U_OVERRIDE;

    bool containsSymbolType(AffixPatternType, UErrorCode&) const U_OVERRIDE;

    bool hasBody() const U_OVERRIDE;

  private:
    PropertiesAffixPatternProvider affixesByPlural[StandardPlural::COUNT];

    bool fBogus{true};
};


/**
 * A struct for ownership of a few objects needed for formatting.
 */
struct DecimalFormatWarehouse {
    PropertiesAffixPatternProvider propertiesAPP;
    CurrencyPluralInfoAffixProvider currencyPluralInfoAPP;
    CurrencySymbols currencySymbols;
};


/**
* Internal fields for DecimalFormat.
* TODO: Make some of these fields by value instead of by LocalPointer?
*/
struct DecimalFormatFields : public UMemory {
    /** The property bag corresponding to user-specified settings and settings from the pattern string. */
    LocalPointer<DecimalFormatProperties> properties;

    /** The symbols for the current locale. */
    LocalPointer<const DecimalFormatSymbols> symbols;

    /**
    * The pre-computed formatter object. Setters cause this to be re-computed atomically. The {@link
    * #format} method uses the formatter directly without needing to synchronize.
    */
    LocalPointer<LocalizedNumberFormatter> formatter;

    /** The lazy-computed parser for .parse() */
    std::atomic<::icu::numparse::impl::NumberParserImpl*> atomicParser = {};

    /** The lazy-computed parser for .parseCurrency() */
    std::atomic<::icu::numparse::impl::NumberParserImpl*> atomicCurrencyParser = {};

    /** Small object ownership warehouse for the formatter and parser */
    DecimalFormatWarehouse warehouse;

    /** The effective properties as exported from the formatter object. Used by some getters. */
    LocalPointer<DecimalFormatProperties> exportedProperties;

    // Data for fastpath
    bool canUseFastFormat = false;
    struct FastFormatData {
        char16_t cpZero;
        char16_t cpGroupingSeparator;
        char16_t cpMinusSign;
        int8_t minInt;
        int8_t maxInt;
    } fastData;
};


/**
 * Utilities for converting between a DecimalFormatProperties and a MacroProps.
 */
class NumberPropertyMapper {
  public:
    /** Convenience method to create a NumberFormatter directly from Properties. */
    static UnlocalizedNumberFormatter create(const DecimalFormatProperties& properties,
                                             const DecimalFormatSymbols& symbols,
                                             DecimalFormatWarehouse& warehouse, UErrorCode& status);

    /** Convenience method to create a NumberFormatter directly from Properties. */
    static UnlocalizedNumberFormatter create(const DecimalFormatProperties& properties,
                                             const DecimalFormatSymbols& symbols,
                                             DecimalFormatWarehouse& warehouse,
                                             DecimalFormatProperties& exportedProperties,
                                             UErrorCode& status);

    /**
     * Creates a new {@link MacroProps} object based on the content of a {@link DecimalFormatProperties}
     * object. In other words, maps Properties to MacroProps. This function is used by the
     * JDK-compatibility API to call into the ICU 60 fluent number formatting pipeline.
     *
     * @param properties
     *            The property bag to be mapped.
     * @param symbols
     *            The symbols associated with the property bag.
     * @param exportedProperties
     *            A property bag in which to store validated properties. Used by some DecimalFormat
     *            getters.
     * @return A new MacroProps containing all of the information in the Properties.
     */
    static MacroProps oldToNew(const DecimalFormatProperties& properties,
                               const DecimalFormatSymbols& symbols, DecimalFormatWarehouse& warehouse,
                               DecimalFormatProperties* exportedProperties, UErrorCode& status);
};


} // namespace impl
} // namespace numparse
U_NAMESPACE_END

#endif //__NUMBER_MAPPER_H__
#endif /* #if !UCONFIG_NO_FORMATTING */
