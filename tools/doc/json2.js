// Copyright Joyent, Inc. and other Node contributors.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so, subject to the
// following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
// NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
// USE OR OTHER DEALINGS IN THE SOFTWARE.

'use strict';

const unified = require('unified');
const common = require('./common.js');
const html = require('remark-html');
const select = require('unist-util-select');

module.exports = doJSON;

// temporary shim
function doJSON(input, fileName, cb) {
  const markdown = require('remark-parse');

  function nullCompiler() {
    this.Compiler = (tree) => tree;
  }

  return unified()
    .use(markdown)
    .use(jsonAPI, { fileName })
    .use(nullCompiler)
    .process(input, cb);
}

// Unified processor: input is https://github.com/syntax-tree/mdast,
// output is: https://gist.github.com/1777387.
function jsonAPI({ fileName }) {
  return (tree, file) => {

    const exampleHeading = /^example/i;
    const metaExpr = /<!--([^=]+)=([^-]+)-->\n*/g;
    const stabilityExpr = /^Stability: ([0-5])(?:\s*-\s*)?(.*)$/s;

    // extract definitions
    const definitions = select(tree, 'definition');

    // Determine the start, stop, and depth of each section.
    const sections = [];
    let section = null;
    tree.children.forEach((node, i) => {
      if (node.type === 'heading' &&
          !exampleHeading.test(textJoin(node.children))) {
        if (section) section.stop = i - 1;
        section = { start: i, stop: tree.children.length, depth: node.depth };
        sections.push(section);
      }
    });

    // Collect and return results.
    const result = { type: 'module', source: fileName };
    while (sections.length > 0) {
      doSection(sections.shift(), result);
    }
    return result;

    // Process a single section (recursively, including subsections).
    function doSection(section, parent) {
      if (section.depth - parent.depth > 1) {
        throw new Error('Inappropriate heading level\n' +
                        JSON.stringify(section));
      }

      const current = newSection(tree.children[section.start]);
      let nodes = tree.children.slice(section.start + 1, section.stop + 1);

      // Sometimes we have two headings with a single blob of description.
      // Treat as a clone.
      if (
        nodes.length === 0 && sections.length > 0 &&
        section.depth === sections[0].depth
      ) {
        nodes = tree.children.slice(sections[0].start + 1,
                                    sections[0].stop + 1);
      }

      // Extract (and remove) metadata that is not directly inferable
      // from the markdown itself.
      nodes.forEach((node, i) => {
        // Input: <!-- name=module -->; output: {name: module}
        if (node.type === 'html') {
          node.value = node.value.replace(metaExpr, (_0, key, value) => {
            current[key.trim()] = value.trim();
            return '';
          });
          if (!node.value.trim()) delete nodes[i];
        }

        // Process metadata:
        // <!-- YAML
        // added: v1.0.0
        // -->
        if (node.type === 'html' && common.isYAMLBlock(node.value)) {
          current.meta = common.extractAndParseYAML(node.value);
          delete nodes[i];
        }

        // Stablility marker: > Stability: ...
        if (
          node.type === 'blockquote' && node.children.length === 1 &&
          node.children[0].type === 'paragraph' &&
          nodes.slice(0, i).every((node) => node.type === 'list')
        ) {
          const text = textJoin(node.children[0].children);
          const stability = text.match(stabilityExpr);
          if (stability) {
            current.stability = parseInt(stability[1], 10);
            current.stabilityText = stability[2].trim();
            delete nodes[i];
          }
        }
      });

      // Compress the node array.
      nodes = nodes.filter(() => true);

      // If the first node is a list, extract it.
      const list = nodes[0] && nodes[0].type === 'list' ?
        nodes.shift() : null;

      // Now figure out what this list actually means.
      // Depending on the section type, the list could be different things.
      if (list) {
        const values = list.children.map((child) => parseListItem(child, file));

        switch (current.type) {
          case 'ctor':
          case 'classMethod':
          case 'method':
            // Each item is an argument, unless the name is 'return',
            // in which case it's the return value.
            const sig = {};
            sig.params = values.filter((value) => {
              if (value.name === 'return') {
                sig.return = value;
                return false;
              }
              return true;
            });
            parseSignature(current.textRaw, sig);
            current.signatures = [sig];
            break;

          case 'property':
            // There should be only one item, which is the value.
            // Copy the data up to the section.
            if (values.length) {
              const signature = values[0];

              // Shove the name in there for properties,
              // since they are always just going to be the value etc.
              signature.textRaw = `\`${current.name}\` ${signature.textRaw}`;

              for (const key in signature) {
                if (signature[key]) {
                  if (key === 'type') {
                    current.typeof = signature.type;
                  } else {
                    current[key] = signature[key];
                  }
                }
              }
            }
            break;

          case 'event':
            // Event: each item is an argument.
            current.params = values;
            break;

          default:
            // If list wasn't consumed, put it back in the nodes list.
            nodes.unshift(list);
        }
      }

      // Convert remaining nodes to a 'desc'.
      // Unified expects to process a string; but we ignore that as we
      // already have pre-parsed input that we can inject.
      if (nodes.length) {
        if (current.desc) current.shortDesc = current.desc;

        current.desc = unified()
          .use(function() {
            this.Parser = () => (
              { type: 'root', children: nodes.concat(definitions) }
            );
          })
          .use(html)
          .processSync('').toString().trim();
        if (!current.desc) delete current.desc;
      }

      // Process subsections.
      while (sections.length > 0 && sections[0].depth > section.depth) {
        doSection(sections.shift(), current);
      }

      // If type is not set, default type based on parent type, and
      // set displayName and name properties.
      if (!current.type) {
        current.type = (parent.type === 'misc' ? 'misc' : 'module');
        current.displayName = current.name;
        current.name = current.name.toLowerCase()
          .trim().replace(/\s+/g, '_');
      }

      // Pluralize type to determine which 'bucket' to put this section
      // in.
      let plur;
      if (current.type.slice(-1) === 's') {
        plur = `${current.type}es`;
      } else if (current.type.slice(-1) === 'y') {
        plur = current.type.replace(/y$/, 'ies');
      } else {
        plur = `${current.type}s`;
      }

      // Classes sometimes have various 'ctor' children
      // which are actually just descriptions of a constructor class signature.
      // Merge them into the parent.
      if (current.type === 'class' && current.ctors) {
        current.signatures = current.signatures || [];
        const sigs = current.signatures;
        current.ctors.forEach((ctor) => {
          ctor.signatures = ctor.signatures || [{}];
          ctor.signatures.forEach((sig) => {
            sig.desc = ctor.desc;
          });
          sigs.push(...ctor.signatures);
        });
        delete current.ctors;
      }

      // Properties are a bit special.
      // Their "type" is the type of object, not "property".
      if (current.type === 'property') {
        if (current.typeof) {
          current.type = current.typeof;
          delete current.typeof;
        } else {
          delete current.type;
        }
      }

      // If the parent's type is 'misc', then it's just a random
      // collection of stuff, like the "globals" section.
      // Make the children top-level items.
      if (current.type === 'misc') {
        Object.keys(current).forEach((key) => {
          switch (key) {
            case 'textRaw':
            case 'name':
            case 'type':
            case 'desc':
            case 'miscs':
              return;
            default:
              if (parent.type === 'misc') {
                return;
              }
              if (parent[key] && Array.isArray(parent[key])) {
                parent[key] = parent[key].concat(current[key]);
              } else if (!parent[key]) {
                parent[key] = current[key];
              }
          }
        });
      }

      // Add this section to the parent.  Sometimes we have two headings with a
      // single blob of description.  If the preceding entry at this level
      // shares a name and is lacking a description, copy it backwards.
      if (!parent[plur]) parent[plur] = [];
      const prev = parent[plur].slice(-1)[0];
      if (prev && prev.name === current.name && !prev.desc) {
        prev.desc = current.desc;
      }
      parent[plur].push(current);
    }
  };
}


const paramExpr = /\((.+)\);?$/;

// text: "someobject.someMethod(a[, b=100][, c])"
function parseSignature(text, sig) {
  let [, sigParams] = text.match(paramExpr) || [];
  if (!sigParams) return;
  sigParams = sigParams.split(',');
  let optionalLevel = 0;
  const optionalCharDict = { '[': 1, ' ': 0, ']': -1 };
  sigParams.forEach((sigParam, i) => {
    sigParam = sigParam.trim();
    if (!sigParam) {
      throw new Error(`Empty parameter slot: ${text}`);
    }
    let listParam = sig.params[i];
    let optional = false;
    let defaultValue;

    // For grouped optional params such as someMethod(a[, b[, c]]).
    let pos;
    for (pos = 0; pos < sigParam.length; pos++) {
      const levelChange = optionalCharDict[sigParam[pos]];
      if (levelChange === undefined) break;
      optionalLevel += levelChange;
    }
    sigParam = sigParam.substring(pos);
    optional = (optionalLevel > 0);
    for (pos = sigParam.length - 1; pos >= 0; pos--) {
      const levelChange = optionalCharDict[sigParam[pos]];
      if (levelChange === undefined) break;
      optionalLevel += levelChange;
    }
    sigParam = sigParam.substring(0, pos + 1);

    const eq = sigParam.indexOf('=');
    if (eq !== -1) {
      defaultValue = sigParam.substr(eq + 1);
      sigParam = sigParam.substr(0, eq);
    }
    if (!listParam) {
      listParam = sig.params[i] = { name: sigParam };
    }
    // At this point, the name should match.
    if (sigParam !== listParam.name) {
      throw new Error(
        `Warning: invalid param "${sigParam}"\n` +
        ` > ${JSON.stringify(listParam)}\n` +
        ` > ${text}`
      );
    }
    if (optional) listParam.optional = true;
    if (defaultValue !== undefined) listParam.default = defaultValue.trim();
  });
}


const returnExpr = /^returns?\s*:?\s*/i;
const nameExpr = /^['`"]?([^'`": {]+)['`"]?\s*:?\s*/;
const typeExpr = /^\{([^}]+)\}\s*/;
const leadingHyphen = /^-\s*/;
const defaultExpr = /\s*\*\*Default:\*\*\s*([^]+)$/i;

function parseListItem(item, file) {
  const current = {};

  current.textRaw = item.children.filter((node) => node.type !== 'list')
    .map((node) => (
      file.contents.slice(node.position.start.offset, node.position.end.offset))
    )
    .join('').replace(/\s+/g, ' ');
  let text = current.textRaw;

  if (!text) {
    throw new Error(`Empty list item: ${JSON.stringify(item)}`);
  }

  // The goal here is to find the name, type, default.
  // Anything left over is 'desc'.

  if (returnExpr.test(text)) {
    current.name = 'return';
    text = text.replace(returnExpr, '');
  } else {
    const [, name] = text.match(nameExpr) || [];
    if (name) {
      current.name = name;
      text = text.replace(nameExpr, '');
    }
  }

  const [, type] = text.match(typeExpr) || [];
  if (type) {
    current.type = type;
    text = text.replace(typeExpr, '');
  }

  text = text.replace(leadingHyphen, '');

  const [, defaultValue] = text.match(defaultExpr) || [];
  if (defaultValue) {
    current.default = defaultValue.replace(/\.$/, '');
    text = text.replace(defaultExpr, '');
  }

  if (text) current.desc = text;

  const options = item.children.find((child) => child.type === 'list');
  if (options) {
    current.options = options.children.map((child) => (
      parseListItem(child, file)
    ));
  }

  return current;
}

// This section parse out the contents of an H# tag.

// To reduse escape slashes in RegExp string components.
const r = String.raw;

const eventPrefix = '^Event: +';
const classPrefix = '^[Cc]lass: +';
const ctorPrefix = '^(?:[Cc]onstructor: +)?new +';
const classMethodPrefix = '^Class Method: +';
const maybeClassPropertyPrefix = '(?:Class Property: +)?';

const maybeQuote = '[\'"]?';
const notQuotes = '[^\'"]+';

// To include constructs like `readable\[Symbol.asyncIterator\]()`
// or `readable.\_read(size)` (with Markdown escapes).
const simpleId = r`(?:(?:\\?_)+|\b)\w+\b`;
const computedId = r`\\?\[[\w\.]+\\?\]`;
const id = `(?:${simpleId}|${computedId})`;
const classId = r`[A-Z]\w+`;

const ancestors = r`(?:${id}\.?)+`;
const maybeAncestors = r`(?:${id}\.?)*`;

const callWithParams = r`\([^)]*\)`;

const noCallOrProp = '(?![.[(])';

const maybeExtends = `(?: +extends +${maybeAncestors}${classId})?`;

const headingExpressions = [
  { type: 'event', re: RegExp(
    `${eventPrefix}${maybeQuote}(${notQuotes})${maybeQuote}$`, 'i') },

  { type: 'class', re: RegExp(
    `${classPrefix}(${maybeAncestors}${classId})${maybeExtends}$`, '') },

  { type: 'ctor', re: RegExp(
    `${ctorPrefix}(${maybeAncestors}${classId})${callWithParams}$`, '') },

  { type: 'classMethod', re: RegExp(
    `${classMethodPrefix}${maybeAncestors}(${id})${callWithParams}$`, 'i') },

  { type: 'method', re: RegExp(
    `^${maybeAncestors}(${id})${callWithParams}$`, 'i') },

  { type: 'property', re: RegExp(
    `^${maybeClassPropertyPrefix}${ancestors}(${id})${noCallOrProp}$`, 'i') },
];

function newSection(header) {
  const text = textJoin(header.children);

  // Infer the type from the text.
  for (const { type, re } of headingExpressions) {
    const [, name] = text.match(re) || [];
    if (name) {
      return { textRaw: text, type, name };
    }
  }
  return { textRaw: text, name: text };
}

function textJoin(nodes) {
  return nodes.map((node) => {
    if (node.type === 'linkReference') {
      return `[${textJoin(node.children)}]` +
        (node.referenceType === 'full' ? `[${node.identifier}]` : '') +
        (node.referenceType === 'collapsed' ? '[]' : '');
    } else if (node.type === 'inlineCode') {
      return `\`${node.value}\``;
    } else if (node.type === 'strong') {
      return `**${textJoin(node.children)}**`;
    } else if (node.type === 'emphasis') {
      return `_${textJoin(node.children)}_`;
    } else if (node.children) {
      return textJoin(node.children);
    } else {
      return node.value;
    }
  }).join('');
}
