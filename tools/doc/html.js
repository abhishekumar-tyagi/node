'use strict';

const common = require('./common.js');
const fs = require('fs');
const marked = require('marked');
const path = require('path');
const preprocess = require('./preprocess.js');
const typeParser = require('./type-parser.js');

const linkManPages = require('./lib/linkManPages')
const parseText = require('./lib/parseText')
const toHTML = require('./lib/toHTML')
const loadGtoc = require('./lib/loadGtoc')

module.exports = toHTML;

// customized heading without id attribute
var renderer = new marked.Renderer();
renderer.heading = function(text, level) {
  return '<h' + level + '>' + text + '</h' + level + '>\n';
};
marked.setOptions({
  renderer: renderer
});


function toID(filename) {
  return filename
    .replace('.html', '')
    .replace(/[^\w\-]/g, '-')
    .replace(/-+/g, '-');
}

function render(lexed, filename, template, nodeVersion, cb) {
  if (typeof nodeVersion === 'function') {
    cb = nodeVersion;
    nodeVersion = null;
  }

  nodeVersion = nodeVersion || process.version;

  // get the section
  var section = getSection(lexed);

  filename = path.basename(filename, '.md');

  parseText(lexed);
  lexed = parseLists(lexed);

  // generate the table of contents.
  // this mutates the lexed contents in-place.
  buildToc(lexed, filename, function(er, toc) {
    if (er) return cb(er);

    var id = toID(path.basename(filename));

    template = template.replace(/__ID__/g, id);
    template = template.replace(/__FILENAME__/g, filename);
    template = template.replace(/__SECTION__/g, section);
    template = template.replace(/__VERSION__/g, nodeVersion);
    template = template.replace(/__TOC__/g, toc);
    template = template.replace(
      /__GTOC__/g,
      gtocData.replace('class="nav-' + id, 'class="nav-' + id + ' active')
    );

    // content has to be the last thing we do with
    // the lexed tokens, because it's destructive.
    const content = marked.parser(lexed);
    template = template.replace(/__CONTENT__/g, content);

    cb(null, template);
  });
}


// just update the list item text in-place.
// lists that come right after a heading are what we're after.
function parseLists(input) {
  var state = null;
  var depth = 0;
  var output = [];
  output.links = input.links;
  input.forEach(function(tok) {
    if (tok.type === 'code' && tok.text.match(/Stability:.*/g)) {
      tok.text = parseAPIHeader(tok.text);
      output.push({ type: 'html', text: tok.text });
      return;
    }
    if (state === null ||
      (state === 'AFTERHEADING' && tok.type === 'heading')) {
      if (tok.type === 'heading') {
        state = 'AFTERHEADING';
      }
      output.push(tok);
      return;
    }
    if (state === 'AFTERHEADING') {
      if (tok.type === 'list_start') {
        state = 'LIST';
        if (depth === 0) {
          output.push({ type: 'html', text: '<div class="signature">' });
        }
        depth++;
        output.push(tok);
        return;
      }
      if (tok.type === 'html' && common.isYAMLBlock(tok.text)) {
        tok.text = parseYAML(tok.text);
      }
      state = null;
      output.push(tok);
      return;
    }
    if (state === 'LIST') {
      if (tok.type === 'list_start') {
        depth++;
        output.push(tok);
        return;
      }
      if (tok.type === 'list_end') {
        depth--;
        output.push(tok);
        if (depth === 0) {
          state = null;
          output.push({ type: 'html', text: '</div>' });
        }
        return;
      }
    }
    output.push(tok);
  });

  return output;
}

function parseYAML(text) {
  const meta = common.extractAndParseYAML(text);
  const html = ['<div class="api_metadata">'];

  if (meta.added) {
    html.push(`<span>Added in: ${meta.added.join(', ')}</span>`);
  }

  if (meta.deprecated) {
    html.push(`<span>Deprecated since: ${meta.deprecated.join(', ')} </span>`);
  }

  html.push('</div>');
  return html.join('\n');
}


function linkJsTypeDocs(text) {
  var parts = text.split('`');
  var i;
  var typeMatches;

  // Handle types, for example the source Markdown might say
  // "This argument should be a {Number} or {String}"
  for (i = 0; i < parts.length; i += 2) {
    typeMatches = parts[i].match(/\{([^\}]+)\}/g);
    if (typeMatches) {
      typeMatches.forEach(function(typeMatch) {
        parts[i] = parts[i].replace(typeMatch, typeParser.toLink(typeMatch));
      });
    }
  }

  //XXX maybe put more stuff here?
  return parts.join('`');
}

function parseAPIHeader(text) {
  text = text.replace(
    /(.*:)\s(\d)([\s\S]*)/,
    '<pre class="api_stability api_stability_$2">$1 $2$3</pre>'
  );
  return text;
}

// section is just the first heading
function getSection(lexed) {
  for (var i = 0, l = lexed.length; i < l; i++) {
    var tok = lexed[i];
    if (tok.type === 'heading') return tok.text;
  }
  return '';
}


function buildToc(lexed, filename, cb) {
  var toc = [];
  var depth = 0;
  lexed.forEach(function(tok) {
    if (tok.type !== 'heading') return;
    if (tok.depth - depth > 1) {
      return cb(new Error('Inappropriate heading level\n' +
                          JSON.stringify(tok)));
    }

    depth = tok.depth;
    var id = getId(filename + '_' + tok.text.trim());
    toc.push(new Array((depth - 1) * 2 + 1).join(' ') +
             '* <a href="#' + id + '">' +
             tok.text + '</a>');
    tok.text += '<span><a class="mark" href="#' + id + '" ' +
                'id="' + id + '">#</a></span>';
  });

  toc = marked.parse(toc.join('\n'));
  cb(null, toc);
}

var idCounters = {};
function getId(text) {
  text = text.toLowerCase();
  text = text.replace(/[^a-z0-9]+/g, '_');
  text = text.replace(/^_+|_+$/, '');
  text = text.replace(/^([^a-z])/, '_$1');
  if (idCounters.hasOwnProperty(text)) {
    text += '_' + (++idCounters[text]);
  } else {
    idCounters[text] = 0;
  }
  return text;
}
