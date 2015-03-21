var common = require('../common');
var assert = require('assert');
var vm = require('vm');
var spawn = require('child_process').spawn;

assert.throws(function() {
  vm.runInDebugContext('*');
}, /SyntaxError/);

assert.throws(function() {
  vm.runInDebugContext({ toString: assert.fail });
}, /AssertionError/);

assert.throws(function() {
  vm.runInDebugContext('throw URIError("BAM")');
}, /URIError/);

assert.throws(function() {
  vm.runInDebugContext('(function(f) { f(f) })(function(f) { f(f) })');
}, /RangeError/);

assert.equal(typeof(vm.runInDebugContext('this')), 'object');
assert.equal(typeof(vm.runInDebugContext('Debug')), 'object');

assert.strictEqual(vm.runInDebugContext(), undefined);
assert.strictEqual(vm.runInDebugContext(0), 0);
assert.strictEqual(vm.runInDebugContext(null), null);
assert.strictEqual(vm.runInDebugContext(undefined), undefined);

// See https://github.com/iojs/io.js/issues/1190, fatal errors should not
// crash the process.
var script = common.fixturesDir + '/vm-run-in-debug-context.js';
var proc = spawn(process.execPath, [script]);
var data = [];
proc.stdout.on('data', assert.fail);
proc.stderr.on('data', data.push.bind(data));
proc.once('exit', common.mustCall(function(exitCode, signalCode) {
  assert.equal(exitCode, 1);
  assert.equal(signalCode, null);
  var haystack = Buffer.concat(data).toString('utf8');
  assert(/SyntaxError: Unexpected token \*/.test(haystack));
}));

var proc = spawn(process.execPath, [script, 'handle-fatal-exception']);
proc.stdout.on('data', assert.fail);
proc.stderr.on('data', assert.fail);
proc.once('exit', common.mustCall(function(exitCode, signalCode) {
  assert.equal(exitCode, 42);
  assert.equal(signalCode, null);
}));
