'use strict';
const common = require('../common');
var assert = require('assert');
var join = require('path').join;
var fs = require('fs');

common.refreshTmpDir();

var repl = require('repl');

var works = [['inner.one'], 'inner.o'];

const putIn = new common.ArrayStream();
const testMe = repl.start('', putIn);


const testFile = [
  'var top = function() {',
  'var inner = {one:1};'
];
var saveFileName = join(common.tmpDir, 'test.save.js');

// // input some data
putIn.run(testFile);
testMe.forceExecute();

// save it to a file
putIn.run(['.save ' + saveFileName]);
testMe.forceExecute();

// the file should have what I wrote
assert.equal(fs.readFileSync(saveFileName, 'utf8'),
             testFile.join('\n') + '\n\n');

// make sure that the REPL data is "correct"
// so when I load it back I know I'm good
putIn.run(['.clear']);
testMe.forceExecute();
putIn.run(testFile);

testMe.complete('inner.o', function(error, data) {
  assert.deepStrictEqual(data, works);
});

// clear the REPL
testMe.forceExecute();
putIn.run(['.clear']);
testMe.forceExecute();

// Load the file back in
putIn.run(['.load ' + saveFileName]);

// make sure that the REPL data is "correct"
testMe.complete('inner.o', function(error, data) {
  assert.deepStrictEqual(data, works);
});

// clear the REPL
putIn.run(['.clear']);
testMe.forceExecute();

var loadFile = join(common.tmpDir, 'file.does.not.exist');

// should not break
putIn.write = function(data) {
  // make sure I get a failed to load message and not some crazy error
  assert.equal(data, 'Failed to load:' + loadFile + '\n');
  // eat me to avoid work
  putIn.write = function() {};
};
putIn.run(['.load ' + loadFile]);
testMe.forceExecute();

// throw error on loading directory
loadFile = common.tmpDir;
putIn.write = function(data) {
  assert.equal(data, 'Failed to load:' + loadFile + ' is not a valid file\n');
  putIn.write = function() {};
};
putIn.run(['.load ' + loadFile]);
testMe.forceExecute();

// clear the REPL
putIn.run(['.clear']);
testMe.forceExecute();

// NUL (\0) is disallowed in filenames in UNIX-like operating systems and
// Windows so we can use that to test failed saves
const invalidFileName = join(common.tmpDir, '\0\0\0\0\0');

// should not break
putIn.write = function(data) {
  // make sure I get a failed to save message and not some other error
  assert.equal(data, 'Failed to save:' + invalidFileName + '\n');
  // reset to no-op
  putIn.write = function() {};
};

// save it to a file
putIn.run(['.save ' + invalidFileName]);
testMe.forceExecute();

{
  // save .editor mode code
  const cmds = [
    'function testSave() {',
    'return "saved";',
    '}'
  ];
  const putIn = new common.ArrayStream();
  const replServer = repl.start({
    prompt: '',
    input: putIn,
    output: putIn,
    terminal: true,
  });

  putIn.run(['.editor']);
  replServer.forceExecute();
  putIn.run(cmds);
  replServer.write('', {ctrl: true, name: 'd'});

  putIn.run([`.save ${saveFileName}`]);
  replServer.forceExecute();
  replServer.close();
  assert.strictEqual(fs.readFileSync(saveFileName, 'utf8'),
                     `${cmds.join('\n')}\n\n`);
}
