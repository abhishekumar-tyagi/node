'use strict';
const common = require('../common');
const assert = require('assert');
const spawn = require('child_process').spawn;
const os = require('os');

const debugPort = common.PORT;
const args = ['--interactive', '--debug-port=' + debugPort];
const childOptions = { stdio: ['pipe', 'pipe', 'pipe', 'ipc'] };
const child = spawn(process.execPath, args, childOptions);

const reDeprecationWarning = new RegExp(
  /^\(node:\d+\) \[DEP0062\] DeprecationWarning: /.source +
  /node --debug is deprecated\. /.source +
  /Please use node --inspect instead\.$/.source
);

child.stdin.write("process.send({ msg: 'childready' });\n");

child.stderr.on('data', function(data) {
  const lines = data.toString().replace(/\r/g, '').trim().split('\n');
  lines.forEach(processStderrLine);
});

child.on('message', function onChildMsg(message) {
  if (message.msg === 'childready') {
    try {
      process._debugProcess(child.pid);
    } catch (e) {
      if (common.isWindows && (e.message ===
            'Not enough storage is available to process this command.' ||
            e.message === 'Access is denied.')) {
        child.kill();
        console.log('Encountered IRPStackSize bug on win32, ignoring test');
        return process.exit();
      }
      throw e;
    }
  }
});

process.on('exit', function() {
  child.kill();
  assertOutputLines();
});

const outputLines = [];
function processStderrLine(line) {
  console.log('> ' + line);
  outputLines.push(line);

  if (/Debugger listening/.test(line)) {
    process.exit();
  }
}

function assertOutputLines() {
  // need a var so can swap the first two lines in following
  // eslint-disable-next-line no-var
  var expectedLines = [
    /^Starting debugger agent\.$/,
    reDeprecationWarning,
    new RegExp(`^Debugger listening on 127\\.0\\.0\\.1:${debugPort}$`)
  ];

  if (os.platform() === 'win32') {
    expectedLines[1] = expectedLines[0];
    expectedLines[0] = reDeprecationWarning;
  }

  assert.strictEqual(outputLines.length, expectedLines.length);
  for (let i = 0; i < expectedLines.length; i++)
    assert(expectedLines[i].test(outputLines[i]));
}
