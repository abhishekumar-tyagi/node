/**
 * @fileoverview Disallow use of multiple spaces.
 * @author Nicholas C. Zakas
 */

"use strict";

const astUtils = require("../ast-utils");

//------------------------------------------------------------------------------
// Rule Definition
//------------------------------------------------------------------------------

module.exports = {
    meta: {
        docs: {
            description: "disallow multiple spaces",
            category: "Best Practices",
            recommended: false
        },

        fixable: "whitespace",

        schema: [
            {
                type: "object",
                properties: {
                    exceptions: {
                        type: "object",
                        patternProperties: {
                            "^([A-Z][a-z]*)+$": {
                                type: "boolean"
                            }
                        },
                        additionalProperties: false
                    },
                    ignoreEOLComments: {
                        type: "boolean"
                    }
                },
                additionalProperties: false
            }
        ]
    },

    create(context) {
        const sourceCode = context.getSourceCode();
        const options = context.options[0] || {};
        const ignoreEOLComments = options.ignoreEOLComments;
        const exceptions = Object.assign({ Property: true }, options.exceptions);
        const hasExceptions = Object.keys(exceptions).filter(key => exceptions[key]).length > 0;

        /**
         * Formats value of given comment token for error message by truncating its length.
         * @param {Token} token comment token
         * @returns {string} formatted value
         * @private
         */
        function formatReportedCommentValue(token) {
            const valueLines = token.value.split("\n");
            const value = valueLines[0];
            const formattedValue = `${value.slice(0, 12)}...`;

            return valueLines.length === 1 && value.length <= 12 ? value : formattedValue;
        }

        //--------------------------------------------------------------------------
        // Public
        //--------------------------------------------------------------------------

        return {
            Program() {
                sourceCode.tokensAndComments.forEach((leftToken, leftIndex, tokensAndComments) => {
                    if (leftIndex === tokensAndComments.length - 1) {
                        return;
                    }
                    const rightToken = tokensAndComments[leftIndex + 1];

                    // Ignore tokens that don't have 2 spaces between them or are on different lines
                    if (
                        !sourceCode.text.slice(leftToken.range[1], rightToken.range[0]).includes("  ") ||
                        leftToken.loc.end.line < rightToken.loc.start.line
                    ) {
                        return;
                    }

                    // Ignore comments that are the last token on their line if `ignoreEOLComments` is active.
                    if (
                        ignoreEOLComments &&
                        astUtils.isCommentToken(rightToken) &&
                        (
                            leftIndex === tokensAndComments.length - 2 ||
                            rightToken.loc.end.line < tokensAndComments[leftIndex + 2].loc.start.line
                        )
                    ) {
                        return;
                    }

                    // Ignore tokens that are in a node in the "exceptions" object
                    if (hasExceptions) {
                        const parentNode = sourceCode.getNodeByRangeIndex(rightToken.range[0] - 1);

                        if (parentNode && exceptions[parentNode.type]) {
                            return;
                        }
                    }

                    let displayValue;

                    if (rightToken.type === "Block") {
                        displayValue = `/*${formatReportedCommentValue(rightToken)}*/`;
                    } else if (rightToken.type === "Line") {
                        displayValue = `//${formatReportedCommentValue(rightToken)}`;
                    } else {
                        displayValue = rightToken.value;
                    }

                    context.report({
                        node: rightToken,
                        loc: rightToken.loc.start,
                        message: "Multiple spaces found before '{{displayValue}}'.",
                        data: { displayValue },
                        fix: fixer => fixer.replaceTextRange([leftToken.range[1], rightToken.range[0]], " ")
                    });
                });
            }
        };

    }
};
