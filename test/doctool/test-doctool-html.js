'use strict';

const common = require('../common');
// The doctool currently uses js-yaml from the tool/node_modules/eslint/ tree.
try {
  require('../../tools/node_modules/eslint/node_modules/js-yaml');
} catch (e) {
  common.skip('missing js-yaml (eslint not present)');
}

const assert = require('assert');
const fs = require('fs');
const path = require('path');
const fixtures = require('../common/fixtures');
const processIncludes = require('../../tools/doc/preprocess.js');
const html = require('../../tools/doc/html.js');

// Test data is a list of objects with two properties.
// The file property is the file path.
// The html property is some html which will be generated by the doctool.
// This html will be stripped of all whitespace because we don't currently
// have an html parser.
const testData = [
  {
    file: fixtures.path('sample_document.md'),
    html: '<ol><li>fish</li><li><p>fish</p></li><li><p>Redfish</p></li>' +
      '<li>Bluefish</li></ol>',
  },
  {
    file: fixtures.path('order_of_end_tags_5873.md'),
    html: '<h3>ClassMethod: Buffer.from(array) <span> ' +
      '<a class="mark" href="#foo_class_method_buffer_from_array" ' +
      'id="foo_class_method_buffer_from_array">#</a> </span> </h3><div' +
      'class="signature"><ul><li><code>array</code><a ' +
      'href="https://developer.mozilla.org/en-US/docs/Web/JavaScript/' +
      'Reference/Global_Objects/Array" class="type">&lt;Array&gt;</a></li>' +
      '</ul></div>',
  },
  {
    file: fixtures.path('doc_with_yaml.md'),
    html: '<h1>Sample Markdown with YAML info' +
      '<span><a class="mark" href="#foo_sample_markdown_with_yaml_info" ' +
      ' id="foo_sample_markdown_with_yaml_info">#</a></span></h1>' +
      '<h2>Foobar<span><a class="mark" href="#foo_foobar" ' +
      'id="foo_foobar">#</a></span></h2>' +
      '<div class="api_metadata"><span>Added in: v1.0.0</span></div> ' +
      '<p>Describe <code>Foobar</code> in more detail here.</p>' +
      '<h2>Foobar II<span><a class="mark" href="#foo_foobar_ii" ' +
      'id="foo_foobar_ii">#</a></span></h2>' +
      '<div class="api_metadata">' +
      '<details class="changelog"><summary>History</summary>' +
      '<table><tr><th>Version</th><th>Changes</th></tr>' +
      '<tr><td>v5.3.0, v4.2.0</td>' +
      '<td><p><span>Added in: v5.3.0, v4.2.0</span></p>' +
      '</td></tr>' +
      '<tr><td>v4.2.0</td><td><p>The <code>error</code> parameter can now be' +
      'an arrow function.</p></td></tr></table></details>' +
      '</div> ' +
      '<p>Describe <code>Foobar II</code> in more detail here.' +
      '<a href="http://man7.org/linux/man-pages/man1/fg.1.html">fg(1)</a></p>' +
      '<h2>Deprecated thingy<span><a class="mark" ' +
      'href="#foo_deprecated_thingy" id="foo_deprecated_thingy">#</a>' +
      '</span></h2>' +
      '<div class="api_metadata"><span>Added in: v1.0.0</span>' +
      '<span>Deprecated since: v2.0.0</span></div><p>Describe ' +
      '<code>Deprecated thingy</code> in more detail here.' +
      '<a href="http://man7.org/linux/man-pages/man1/fg.1p.html">fg(1p)</a>' +
      '</p>' +
      '<h2>Something<span><a class="mark" href="#foo_something" ' +
      'id="foo_something">#</a></span></h2> ' +
      '<!-- This is not a metadata comment --> ' +
      '<p>Describe <code>Something</code> in more detail here. ' +
      '</p>',
  },
  {
    file: fixtures.path('doc_with_includes.md'),
    html: '<!-- [start-include:doc_inc_1.md] -->' +
    '<p>Look <a href="doc_inc_2.html#doc_inc_2_foobar">here</a>!</p>' +
    '<!-- [end-include:doc_inc_1.md] -->' +
    '<!-- [start-include:doc_inc_2.md] -->' +
    '<h1>foobar<span><a class="mark" href="#doc_inc_2_foobar" ' +
    'id="doc_inc_2_foobar">#</a></span></h1>' +
    '<p>I exist and am being linked to.</p>' +
    '<!-- [end-include:doc_inc_2.md] -->',
  },
  {
    file: fixtures.path('sample_document.md'),
    html: '<ol><li>fish</li><li><p>fish</p></li><li><p>Redfish</p></li>' +
      '<li>Bluefish</li></ol>',
    analyticsId: 'UA-67020396-1',
  },
];

const spaces = /\s/g;

testData.forEach((item) => {
  // Normalize expected data by stripping whitespace
  const expected = item.html.replace(spaces, '');
  const includeAnalytics = typeof item.analyticsId !== 'undefined';

  fs.readFile(item.file, 'utf8', common.mustCall((err, input) => {
    assert.ifError(err);
    processIncludes(item.file, input, common.mustCall((err, preprocessed) => {
      assert.ifError(err);

      html(
        {
          input: preprocessed,
          filename: 'foo',
          template: path.resolve(__dirname, '../../doc/template.html'),
          nodeVersion: process.version,
          analytics: item.analyticsId,
        },
        common.mustCall((err, output) => {
          assert.ifError(err);

          const actual = output.replace(spaces, '');
          const scriptDomain = 'google-analytics.com';
          // Assert that the input stripped of all whitespace contains the
          // expected list
          assert(actual.includes(expected));

          // Testing the insertion of Google Analytics script when
          // an analytics id is provided. Should not be present by default
          if (includeAnalytics) {
            assert(actual.includes(scriptDomain),
                   `Google Analytics script was not present in "${actual}"`);
          } else {
            assert.strictEqual(actual.includes(scriptDomain), false,
                               'Google Analytics script was present in ' +
                               `"${actual}"`);
          }
        }));
    }));
  }));
});
