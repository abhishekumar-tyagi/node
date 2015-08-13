'use strict';

// Flags: --expose-internals

const common = require('../common');
const stream = require('stream');
const REPL = require('internal/repl');
const assert = require('assert');
const fs = require('fs');
const util = require('util');
const path = require('path');
const os = require('os');

common.refreshTmpDir();

// Mock os.homedir()
os.homedir = function() {
  return common.tmpDir;
};

// Create an input stream specialized for testing an array of actions
class ActionStream extends stream.Stream {
  run(data) {
    const _iter = data[Symbol.iterator]();
    const self = this;

    function doAction() {
      const next = _iter.next();
      if (next.done) {
        // Close the repl. Note that it must have a clean prompt to do so.
        setImmediate(function() {
          self.emit('keypress', '', { ctrl: true, name: 'd' });
        });
        return;
      }
      const action = next.value;

      if (typeof action === 'object') {
        self.emit('keypress', '', action);
      } else {
        self.emit('data', action + '\n');
      }
      setImmediate(doAction);
    }
    setImmediate(doAction);
  }
  resume() {}
  pause() {}
}
ActionStream.prototype.readable = true;


// Mock keys
const UP = { name: 'up' };
const ENTER = { name: 'enter' };
const CLEAR = { ctrl: true, name: 'u' };
// Common message bits
const prompt = '> ';
const replDisabled = '\nPersistent history support disabled. Set the ' +
                     'NODE_REPL_HISTORY environment\nvariable to a valid, ' +
                     'user-writable path to enable.\n';
const convertMsg = '\nConverting old JSON repl history to line-separated ' +
                   'history.\nThe new repl history file can be found at ' +
                   path.join(common.tmpDir, '.node_repl_history') + '.\n';
const homedirErr = '\nError: Could not get the home directory.\n' +
                   'REPL session history will not be persisted.\n';
// File paths
const fixtures = path.join(common.testDir, 'fixtures');
const historyFixturePath = path.join(fixtures, '.node_repl_history');
const historyPath = path.join(common.tmpDir, '.fixture_copy_repl_history');
const oldHistoryPath = path.join(fixtures, 'old-repl-history-file.json');


const tests = [{
  env: { NODE_REPL_HISTORY: '' },
  test: [UP],
  expected: [prompt, replDisabled, prompt],
  event: 'end'
},
{
  env: { NODE_REPL_HISTORY: '',
         NODE_REPL_HISTORY_FILE: oldHistoryPath },
  test: [UP],
  expected: [prompt, replDisabled, prompt],
  event: 'end'
},
{
  env: { NODE_REPL_HISTORY: historyPath },
  test: [UP, CLEAR],
  expected: [prompt, prompt + '\'you look fabulous today\'', prompt],
  event: 'end'
},
{
  env: { NODE_REPL_HISTORY: historyPath,
         NODE_REPL_HISTORY_FILE: oldHistoryPath },
  test: [UP, CLEAR],
  expected: [prompt, prompt + '\'you look fabulous today\'', prompt],
  event: 'end'
},
{
  env: { NODE_REPL_HISTORY: historyPath,
         NODE_REPL_HISTORY_FILE: '' },
  test: [UP, CLEAR],
  expected: [prompt, prompt + '\'you look fabulous today\'', prompt],
  event: 'end'
},
{
  env: {},
  test: [UP],
  expected: [prompt],
  event: 'end'
},
{
  env: { NODE_REPL_HISTORY_FILE: oldHistoryPath },
  test: [UP, CLEAR, '\'42\'', ENTER/*, function(cb) {
    // XXX(Fishrock123) Allow the REPL to save to disk.
    // There isn't a way to do this programmatically right now.
    setTimeout(cb, 50);
  }*/],
  expected: [prompt, convertMsg, prompt, prompt + '\'=^.^=\'', prompt, '\'',
             '4', '2', '\'', '\'42\'\n', prompt, prompt],
  after: function ensureHistoryFixture() {
    // XXX(Fishrock123) Make sure nothing weird happened to our fixture
    //  or it's temporary copy.
    // Sometimes this test used to erase the fixture and I'm not sure why.
    const history = fs.readFileSync(historyFixturePath, 'utf8');
    assert.strictEqual(history,
                       '\'you look fabulous today\'\n\'Stay Fresh~\'\n');
    const historyCopy = fs.readFileSync(historyPath, 'utf8');
    assert.strictEqual(historyCopy, '\'you look fabulous today\'' + os.EOL +
                                    '\'Stay Fresh~\'' + os.EOL);
  },
  event: 'flushHistory'
},
{
  env: {},
  test: [UP, UP, ENTER],
  expected: [prompt, prompt + '\'42\'', prompt + '\'=^.^=\'', '\'=^.^=\'\n',
             prompt],
  event: 'flushHistory'
},
{ // Make sure this is always the last test, since we change os.homedir()
  before: function mockHomedirFailure() {
    // Mock os.homedir() failure
    os.homedir = function() {
      throw new Error('os.homedir() failure');
    };
  },
  env: {},
  test: [UP],
  expected: [prompt, homedirErr, prompt, replDisabled, prompt],
  event: 'end'
}];


// Copy our fixture to the tmp directory
fs.createReadStream(historyFixturePath)
  .pipe(fs.createWriteStream(historyPath)).on('unpipe', runTest);

function runTest() {
  const opts = tests.shift();
  if (!opts) return; // All done

  const env = opts.env;
  const test = opts.test;
  const expected = opts.expected;
  const after = opts.after;
  const before = opts.before;

  if (before) before();

  REPL.createInternalRepl(env, {
    input: new ActionStream(),
    output: new stream.Writable({
      write(chunk, _, next) {
        const output = chunk.toString();

        // Ignore escapes and blank lines
        if (output.charCodeAt(0) === 27 || /^[\r\n]+$/.test(output))
          return next();

        assert.strictEqual(output, expected.shift());
        next();
      }
    }),
    prompt: prompt,
    useColors: false,
    terminal: true
  }, function(err, repl) {
    if (err) throw err;

    if (after) repl.on(opts.event, after);

    repl.on(opts.event, function() {
      // Ensure everything that we expected was output
      assert.strictEqual(expected.length, 0);
      setImmediate(runTest);
    });

    repl.inputStream.run(test);
  });
}
