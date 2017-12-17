/**
 * @fileoverview Check that TypeError[ERR_INVALID_ARG_TYPE] uses
 *               lowercase for primitive types
 * @author Weijia Wang <starkwang@126.com>
 */
'use strict';

//------------------------------------------------------------------------------
// Rule Definition
//------------------------------------------------------------------------------

const astSelector = 'NewExpression[callee.property.name="TypeError"]' +
                    '[arguments.0.value="ERR_INVALID_ARG_TYPE"]';

const primitives = [
  'number', 'string', 'boolean', 'null', 'undefined'
];

module.exports = function(context) {
  function checkNamesArgument(node) {
    const names = node.arguments[2];

    switch (names.type) {
      case 'Literal':
        checkName(names, names.value);
        break;
      case 'ArrayExpression':
        names.elements.forEach((name, index) => {
          checkName(names.elements[index], name.value);
        });
        break;
    }
  }

  function checkName(node, name) {
    const lowercaseName = name.toLowerCase();
    if (primitives.includes(lowercaseName) && !primitives.includes(name)) {
      const msg = `primitive should use lowercase: ${name}`;
      context.report({
        node,
        message: msg,
        fix: (fixer) => {
          return fixer.replaceText(
            node,
            `'${lowercaseName}'`
          );
        }
      });
    }

  }

  return {
    [astSelector]: (node) => checkNamesArgument(node)
  };
};
