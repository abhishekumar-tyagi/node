/**
 * @fileoverview A rule to suggest using of const declaration for variables that are never reassigned after declared.
 * @author Toru Nagashima
 */

"use strict";

//------------------------------------------------------------------------------
// Requirements
//------------------------------------------------------------------------------

var Map = require("es6-map");

//------------------------------------------------------------------------------
// Helpers
//------------------------------------------------------------------------------

var PATTERN_TYPE = /^(?:.+?Pattern|RestElement|Property)$/;
var DECLARATION_HOST_TYPE = /^(?:Program|BlockStatement|SwitchCase)$/;
var DESTRUCTURING_HOST_TYPE = /^(?:VariableDeclarator|AssignmentExpression)$/;

/**
 * Adds multiple items to the tail of an array.
 *
 * @param {any[]} array - A destination to add.
 * @param {any[]} values - Items to be added.
 * @returns {void}
 */
var pushAll = Function.apply.bind(Array.prototype.push);

/**
 * Checks whether a given node is located at `ForStatement.init` or not.
 *
 * @param {ASTNode} node - A node to check.
 * @returns {boolean} `true` if the node is located at `ForStatement.init`.
 */
function isInitOfForStatement(node) {
    return node.parent.type === "ForStatement" && node.parent.init === node;
}

/**
 * Checks whether a given Identifier node becomes a VariableDeclaration or not.
 *
 * @param {ASTNode} identifier - An Identifier node to check.
 * @returns {boolean} `true` if the node can become a VariableDeclaration.
 */
function canBecomeVariableDeclaration(identifier) {
    var node = identifier.parent;

    while (PATTERN_TYPE.test(node.type)) {
        node = node.parent;
    }

    return (
        node.type === "VariableDeclarator" ||
        (
            node.type === "AssignmentExpression" &&
            node.parent.type === "ExpressionStatement" &&
            DECLARATION_HOST_TYPE.test(node.parent.parent.type)
        )
    );
}

/**
 * Gets an identifier node of a given variable.
 *
 * If the initialization exists or one or more reading references exist before
 * the first assignment, the identifier node is the node of the declaration.
 * Otherwise, the identifier node is the node of the first assignment.
 *
 * If the variable should not change to const, this function returns null.
 * - If the variable is reassigned.
 * - If the variable is never initialized and assigned.
 * - If the variable is initialized in a different scope from the declaration.
 * - If the unique assignment of the variable cannot change to a declaration.
 *
 * @param {escope.Variable} variable - A variable to get.
 * @param {boolean} ignoreReadBeforeAssign -
 *      The value of `ignoreReadBeforeAssign` option.
 * @returns {ASTNode|null}
 *      An Identifier node if the variable should change to const.
 *      Otherwise, null.
 */
function getIdentifierIfShouldBeConst(variable, ignoreReadBeforeAssign) {
    if (variable.eslintUsed) {
        return null;
    }

    // Finds the unique WriteReference.
    var writer = null;
    var isReadBeforeInit = false;
    var references = variable.references;

    for (var i = 0; i < references.length; ++i) {
        var reference = references[i];

        if (reference.isWrite()) {
            var isReassigned = (
                writer !== null &&
                writer.identifier !== reference.identifier
            );

            if (isReassigned) {
                return null;
            }
            writer = reference;

        } else if (reference.isRead() && writer === null) {
            if (ignoreReadBeforeAssign) {
                return null;
            }
            isReadBeforeInit = true;
        }
    }

    // If the assignment is from a different scope, ignore it.
    // If the assignment cannot change to a declaration, ignore it.
    var shouldBeConst = (
        writer !== null &&
        writer.from === variable.scope &&
        canBecomeVariableDeclaration(writer.identifier)
    );

    if (!shouldBeConst) {
        return null;
    }
    if (isReadBeforeInit) {
        return variable.defs[0].name;
    }
    return writer.identifier;
}

/**
 * Gets the VariableDeclarator/AssignmentExpression node that a given reference
 * belongs to.
 * This is used to detect a mix of reassigned and never reassigned in a
 * destructuring.
 *
 * @param {escope.Reference} reference - A reference to get.
 * @returns {ASTNode|null} A VariableDeclarator/AssignmentExpression node or
 *      null.
 */
function getDestructuringHost(reference) {
    if (!reference.isWrite()) {
        return null;
    }
    var node = reference.identifier.parent;

    while (PATTERN_TYPE.test(node.type)) {
        node = node.parent;
    }

    if (!DESTRUCTURING_HOST_TYPE.test(node.type)) {
        return null;
    }
    return node;
}

/**
 * Groups by the VariableDeclarator/AssignmentExpression node that each
 * reference of given variables belongs to.
 * This is used to detect a mix of reassigned and never reassigned in a
 * destructuring.
 *
 * @param {escope.Variable[]} variables - Variables to group by destructuring.
 * @param {boolean} ignoreReadBeforeAssign -
 *      The value of `ignoreReadBeforeAssign` option.
 * @returns {Map<ASTNode, ASTNode[]>} Grouped identifier nodes.
 */
function groupByDestructuring(variables, ignoreReadBeforeAssign) {
    var identifierMap = new Map();

    for (var i = 0; i < variables.length; ++i) {
        var variable = variables[i];
        var references = variable.references;
        var identifier = getIdentifierIfShouldBeConst(variable, ignoreReadBeforeAssign);
        var prevId = null;

        for (var j = 0; j < references.length; ++j) {
            var reference = references[j];
            var id = reference.identifier;

            // Avoid counting a reference twice or more for default values of
            // destructuring.
            if (id === prevId) {
                continue;
            }
            prevId = id;

            // Add the identifier node into the destructuring group.
            var group = getDestructuringHost(reference);

            if (group) {
                if (identifierMap.has(group)) {
                    identifierMap.get(group).push(identifier);
                } else {
                    identifierMap.set(group, [identifier]);
                }
            }
        }
    }

    return identifierMap;
}

//------------------------------------------------------------------------------
// Rule Definition
//------------------------------------------------------------------------------

module.exports = {
    meta: {
        docs: {
            description: "require `const` declarations for variables that are never reassigned after declared",
            category: "ECMAScript 6",
            recommended: false
        },

        schema: [
            {
                type: "object",
                properties: {
                    destructuring: {enum: ["any", "all"]},
                    ignoreReadBeforeAssign: {type: "boolean"}
                },
                additionalProperties: false
            }
        ]
    },

    create: function(context) {
        var options = context.options[0] || {};
        var checkingMixedDestructuring = options.destructuring !== "all";
        var ignoreReadBeforeAssign = options.ignoreReadBeforeAssign === true;
        var variables = null;

        /**
         * Reports a given Identifier node.
         *
         * @param {ASTNode} node - An Identifier node to report.
         * @returns {void}
         */
        function report(node) {
            context.report({
                node: node,
                message: "'{{name}}' is never reassigned, use 'const' instead.",
                data: node
            });
        }

        /**
         * Reports a given variable if the variable should be declared as const.
         *
         * @param {escope.Variable} variable - A variable to report.
         * @returns {void}
         */
        function checkVariable(variable) {
            var node = getIdentifierIfShouldBeConst(variable, ignoreReadBeforeAssign);

            if (node) {
                report(node);
            }
        }

        /**
         * Reports given identifier nodes if all of the nodes should be declared
         * as const.
         *
         * The argument 'nodes' is an array of Identifier nodes.
         * This node is the result of 'getIdentifierIfShouldBeConst()', so it's
         * nullable. In simple declaration or assignment cases, the length of
         * the array is 1. In destructuring cases, the length of the array can
         * be 2 or more.
         *
         * @param {(escope.Reference|null)[]} nodes -
         *      References which are grouped by destructuring to report.
         * @returns {void}
         */
        function checkGroup(nodes) {
            if (nodes.every(Boolean)) {
                nodes.forEach(report);
            }
        }

        return {
            Program: function() {
                variables = [];
            },

            "Program:exit": function() {
                if (checkingMixedDestructuring) {
                    variables.forEach(checkVariable);
                } else {
                    groupByDestructuring(variables, ignoreReadBeforeAssign)
                        .forEach(checkGroup);
                }

                variables = null;
            },

            VariableDeclaration: function(node) {
                if (node.kind === "let" && !isInitOfForStatement(node)) {
                    pushAll(variables, context.getDeclaredVariables(node));
                }
            }
        };
    }
};
