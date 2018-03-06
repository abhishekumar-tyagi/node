/**
 * @fileoverview Disallow redundant return statements
 * @author Teddy Katz
 */
"use strict";

//------------------------------------------------------------------------------
// Requirements
//------------------------------------------------------------------------------

const astUtils = require("../ast-utils"),
    FixTracker = require("../util/fix-tracker");

//------------------------------------------------------------------------------
// Helpers
//------------------------------------------------------------------------------

/**
 * Adds all elements of 2nd argument into 1st argument.
 *
 * @param {Array} array - The destination array to add.
 * @param {Array} elements - The source array to add.
 * @returns {void}
 */
const pushAll = Function.apply.bind(Array.prototype.push);

/**
 * Removes the given element from the array.
 *
 * @param {Array} array - The source array to remove.
 * @param {any} element - The target item to remove.
 * @returns {void}
 */
function remove(array, element) {
    const index = array.indexOf(element);

    if (index !== -1) {
        array.splice(index, 1);
    }
}

/**
 * Checks whether it can remove the given return statement or not.
 *
 * @param {ASTNode} node - The return statement node to check.
 * @returns {boolean} `true` if the node is removeable.
 */
function isRemovable(node) {
    return astUtils.STATEMENT_LIST_PARENTS.has(node.parent.type);
}

/**
 * Checks whether the given return statement is in a `finally` block or not.
 *
 * @param {ASTNode} node - The return statement node to check.
 * @returns {boolean} `true` if the node is in a `finally` block.
 */
function isInFinally(node) {
    while (node && node.parent && !astUtils.isFunction(node)) {
        if (node.parent.type === "TryStatement" && node.parent.finalizer === node) {
            return true;
        }

        node = node.parent;
    }

    return false;
}

//------------------------------------------------------------------------------
// Rule Definition
//------------------------------------------------------------------------------

module.exports = {
    meta: {
        docs: {
            description: "disallow redundant return statements",
            category: "Best Practices",
            recommended: false,
            url: "https://eslint.org/docs/rules/no-useless-return"
        },
        fixable: "code",
        schema: []
    },

    create(context) {
        const segmentInfoMap = new WeakMap();
        const usedUnreachableSegments = new WeakSet();
        let scopeInfo = null;

        /**
         * Checks whether the given segment is terminated by a return statement or not.
         *
         * @param {CodePathSegment} segment - The segment to check.
         * @returns {boolean} `true` if the segment is terminated by a return statement, or if it's still a part of unreachable.
         */
        function isReturned(segment) {
            const info = segmentInfoMap.get(segment);

            return !info || info.returned;
        }

        /**
         * Collects useless return statements from the given previous segments.
         *
         * A previous segment may be an unreachable segment.
         * In that case, the information object of the unreachable segment is not
         * initialized because `onCodePathSegmentStart` event is not notified for
         * unreachable segments.
         * This goes to the previous segments of the unreachable segment recursively
         * if the unreachable segment was generated by a return statement. Otherwise,
         * this ignores the unreachable segment.
         *
         * This behavior would simulate code paths for the case that the return
         * statement does not exist.
         *
         * @param {ASTNode[]} uselessReturns - The collected return statements.
         * @param {CodePathSegment[]} prevSegments - The previous segments to traverse.
         * @param {WeakSet<CodePathSegment>} [traversedSegments] A set of segments that have already been traversed in this call
         * @returns {ASTNode[]} `uselessReturns`.
         */
        function getUselessReturns(uselessReturns, prevSegments, traversedSegments) {
            if (!traversedSegments) {
                traversedSegments = new WeakSet();
            }
            for (const segment of prevSegments) {
                if (!segment.reachable) {
                    if (!traversedSegments.has(segment)) {
                        traversedSegments.add(segment);
                        getUselessReturns(
                            uselessReturns,
                            segment.allPrevSegments.filter(isReturned),
                            traversedSegments
                        );
                    }
                    continue;
                }

                pushAll(uselessReturns, segmentInfoMap.get(segment).uselessReturns);
            }

            return uselessReturns;
        }

        /**
         * Removes the return statements on the given segment from the useless return
         * statement list.
         *
         * This segment may be an unreachable segment.
         * In that case, the information object of the unreachable segment is not
         * initialized because `onCodePathSegmentStart` event is not notified for
         * unreachable segments.
         * This goes to the previous segments of the unreachable segment recursively
         * if the unreachable segment was generated by a return statement. Otherwise,
         * this ignores the unreachable segment.
         *
         * This behavior would simulate code paths for the case that the return
         * statement does not exist.
         *
         * @param {CodePathSegment} segment - The segment to get return statements.
         * @returns {void}
         */
        function markReturnStatementsOnSegmentAsUsed(segment) {
            if (!segment.reachable) {
                usedUnreachableSegments.add(segment);
                segment.allPrevSegments
                    .filter(isReturned)
                    .filter(prevSegment => !usedUnreachableSegments.has(prevSegment))
                    .forEach(markReturnStatementsOnSegmentAsUsed);
                return;
            }

            const info = segmentInfoMap.get(segment);

            for (const node of info.uselessReturns) {
                remove(scopeInfo.uselessReturns, node);
            }
            info.uselessReturns = [];
        }

        /**
         * Removes the return statements on the current segments from the useless
         * return statement list.
         *
         * This function will be called at every statement except FunctionDeclaration,
         * BlockStatement, and BreakStatement.
         *
         * - FunctionDeclarations are always executed whether it's returned or not.
         * - BlockStatements do nothing.
         * - BreakStatements go the next merely.
         *
         * @returns {void}
         */
        function markReturnStatementsOnCurrentSegmentsAsUsed() {
            scopeInfo
                .codePath
                .currentSegments
                .forEach(markReturnStatementsOnSegmentAsUsed);
        }

        //----------------------------------------------------------------------
        // Public
        //----------------------------------------------------------------------

        return {

            // Makes and pushs a new scope information.
            onCodePathStart(codePath) {
                scopeInfo = {
                    upper: scopeInfo,
                    uselessReturns: [],
                    codePath
                };
            },

            // Reports useless return statements if exist.
            onCodePathEnd() {
                for (const node of scopeInfo.uselessReturns) {
                    context.report({
                        node,
                        loc: node.loc,
                        message: "Unnecessary return statement.",
                        fix(fixer) {
                            if (isRemovable(node)) {

                                /*
                                 * Extend the replacement range to include the
                                 * entire function to avoid conflicting with
                                 * no-else-return.
                                 * https://github.com/eslint/eslint/issues/8026
                                 */
                                return new FixTracker(fixer, context.getSourceCode())
                                    .retainEnclosingFunction(node)
                                    .remove(node);
                            }
                            return null;
                        }
                    });
                }

                scopeInfo = scopeInfo.upper;
            },

            /*
             * Initializes segments.
             * NOTE: This event is notified for only reachable segments.
             */
            onCodePathSegmentStart(segment) {
                const info = {
                    uselessReturns: getUselessReturns([], segment.allPrevSegments),
                    returned: false
                };

                // Stores the info.
                segmentInfoMap.set(segment, info);
            },

            // Adds ReturnStatement node to check whether it's useless or not.
            ReturnStatement(node) {
                if (node.argument) {
                    markReturnStatementsOnCurrentSegmentsAsUsed();
                }
                if (node.argument || astUtils.isInLoop(node) || isInFinally(node)) {
                    return;
                }

                for (const segment of scopeInfo.codePath.currentSegments) {
                    const info = segmentInfoMap.get(segment);

                    if (info) {
                        info.uselessReturns.push(node);
                        info.returned = true;
                    }
                }
                scopeInfo.uselessReturns.push(node);
            },

            /*
             * Registers for all statement nodes except FunctionDeclaration, BlockStatement, BreakStatement.
             * Removes return statements of the current segments from the useless return statement list.
             */
            ClassDeclaration: markReturnStatementsOnCurrentSegmentsAsUsed,
            ContinueStatement: markReturnStatementsOnCurrentSegmentsAsUsed,
            DebuggerStatement: markReturnStatementsOnCurrentSegmentsAsUsed,
            DoWhileStatement: markReturnStatementsOnCurrentSegmentsAsUsed,
            EmptyStatement: markReturnStatementsOnCurrentSegmentsAsUsed,
            ExpressionStatement: markReturnStatementsOnCurrentSegmentsAsUsed,
            ForInStatement: markReturnStatementsOnCurrentSegmentsAsUsed,
            ForOfStatement: markReturnStatementsOnCurrentSegmentsAsUsed,
            ForStatement: markReturnStatementsOnCurrentSegmentsAsUsed,
            IfStatement: markReturnStatementsOnCurrentSegmentsAsUsed,
            ImportDeclaration: markReturnStatementsOnCurrentSegmentsAsUsed,
            LabeledStatement: markReturnStatementsOnCurrentSegmentsAsUsed,
            SwitchStatement: markReturnStatementsOnCurrentSegmentsAsUsed,
            ThrowStatement: markReturnStatementsOnCurrentSegmentsAsUsed,
            TryStatement: markReturnStatementsOnCurrentSegmentsAsUsed,
            VariableDeclaration: markReturnStatementsOnCurrentSegmentsAsUsed,
            WhileStatement: markReturnStatementsOnCurrentSegmentsAsUsed,
            WithStatement: markReturnStatementsOnCurrentSegmentsAsUsed,
            ExportNamedDeclaration: markReturnStatementsOnCurrentSegmentsAsUsed,
            ExportDefaultDeclaration: markReturnStatementsOnCurrentSegmentsAsUsed,
            ExportAllDeclaration: markReturnStatementsOnCurrentSegmentsAsUsed
        };
    }
};
