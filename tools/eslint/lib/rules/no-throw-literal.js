/**
 * @fileoverview Rule to restrict what can be thrown as an exception.
 * @author Dieter Oberkofler
 */

"use strict";

//------------------------------------------------------------------------------
// Helpers
//------------------------------------------------------------------------------

/**
 * Determine if a node has a possiblity to be an Error object
 * @param  {ASTNode}  node  ASTNode to check
 * @returns {boolean}       True if there is a chance it contains an Error obj
 */
function couldBeError(node) {
    switch (node.type) {
        case "Identifier":
        case "CallExpression":
        case "NewExpression":
        case "MemberExpression":
        case "TaggedTemplateExpression":
        case "YieldExpression":
            return true; // possibly an error object.

        case "AssignmentExpression":
            return couldBeError(node.right);

        case "SequenceExpression": {
            const exprs = node.expressions;

            return exprs.length !== 0 && couldBeError(exprs[exprs.length - 1]);
        }

        case "LogicalExpression":
            return couldBeError(node.left) || couldBeError(node.right);

        case "ConditionalExpression":
            return couldBeError(node.consequent) || couldBeError(node.alternate);

        default:
            return false;
    }
}

//------------------------------------------------------------------------------
// Rule Definition
//------------------------------------------------------------------------------

module.exports = {
    meta: {
        docs: {
            description: "disallow throwing literals as exceptions",
            category: "Best Practices",
            recommended: false
        },

        schema: []
    },

    create(context) {

        return {

            ThrowStatement(node) {
                if (!couldBeError(node.argument)) {
                    context.report({ node, message: "Expected an object to be thrown." });
                } else if (node.argument.type === "Identifier") {
                    if (node.argument.name === "undefined") {
                        context.report({ node, message: "Do not throw undefined." });
                    }
                }

            }

        };

    }
};
