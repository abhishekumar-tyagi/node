'use strict';

const fs = require('fs');
const path = require('path');
const marked = require('marked');

const rootDir = path.resolve(__dirname, '..', '..');
const doc = path.resolve(rootDir, 'doc', 'api', 'addons.md');
const verifyDir = path.resolve(rootDir, 'test', 'addons');

const contents = fs.readFileSync(doc).toString();

const tokens = marked.lexer(contents);
let files = null;
let id = 0;

// Just to make sure that all examples will be processed
tokens.push({ type: 'heading' });

for (var i = 0; i < tokens.length; i++) {
  var token = tokens[i];
  if (token.type === 'heading' && token.text) {
    const blockName = token.text;
    if (files && Object.keys(files).length !== 0) {
      verifyFiles(files,
                  blockName,
                  console.log.bind(null, 'wrote'),
                  function(err) { if (err) throw err; });
    }
    files = {};
  } else if (token.type === 'code') {
    var match = token.text.match(/^\/\/\s+(.*\.(?:cc|h|js))[\r\n]/);
    if (match === null)
      continue;
    files[match[1]] = token.text;
  }
}

function once(fn) {
  var once = false;
  return function() {
    if (once)
      return;
    once = true;
    fn.apply(this, arguments);
  };
}

function verifyFiles(files, blockName, onprogress, ondone) {
  // must have a .cc and a .js to be a valid test
  if (!Object.keys(files).some((name) => /\.cc$/.test(name)) ||
      !Object.keys(files).some((name) => /\.js$/.test(name))) {
    return;
  }

  blockName = blockName
    .toLowerCase()
    .replace(/\s/g, '_')
    .replace(/[^a-z\d_]/g, '');
  const dir = path.resolve(
    verifyDir,
    `${(++id < 10 ? '0' : '') + id}_${blockName}`
  );

  // must have a .cc and a .js to be a valid test
  if (!Object.keys(files).some((name) => /\.cc$/.test(name)) ||
      !Object.keys(files).some((name) => /\.js$/.test(name))) {
    return;
  }

  files = Object.keys(files).map(function(name) {
    if (name === 'test.js') {
      files[name] = `'use strict';
const common = require('../../common');
${files[name].replace('Release', "' + common.buildType + '")}
`;
    }
    return {
      path: path.resolve(dir, name),
      name: name,
      content: files[name]
    };
  });

  files.push({
    path: path.resolve(dir, 'binding.gyp'),
    content: JSON.stringify({
      targets: [
        {
          target_name: 'addon',
          defines: [ 'V8_DEPRECATION_WARNINGS=1' ],
          sources: files.map(function(file) {
            return file.name;
          })
        }
      ]
    })
  });

  fs.mkdir(dir, function() {
    // Ignore errors

    var done = once(ondone);
    var waiting = files.length;
    files.forEach(function(file) {
      fs.writeFile(file.path, file.content, function(err) {
        if (err)
          return done(err);

        if (onprogress)
          onprogress(file.path);

        if (--waiting === 0)
          done();
      });
    });
  });
}
