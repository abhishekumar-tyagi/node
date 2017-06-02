'use strict';
const common = require('../common');
const assert = require('assert');
const child_process = require('child_process');
const { promisify } = require('util');

common.crashOnUnhandledRejection();

const exec = promisify(child_process.exec);
const execFile = promisify(child_process.execFile);

{
  exec(`${process.execPath} -p 42`).then(common.mustCall((obj) => {
    assert.deepStrictEqual(obj, { stdout: '42\n', stderr: '' });
  }));
}

{
  execFile(process.execPath, ['-p', '42']).then(common.mustCall((obj) => {
    assert.deepStrictEqual(obj, { stdout: '42\n', stderr: '' });
  }));
}

{
  exec('doesntexist').catch(common.mustCall((err) => {
    assert(err.message.includes('doesntexist'));
  }));
}

{
  execFile('doesntexist', ['-p', '42']).catch(common.mustCall((err) => {
    assert(err.message.includes('doesntexist'));
  }));
}
const failingCodeWithStdoutErr =
  'console.log("out");console.error("err");process.exit(1)';
{
  exec(`${process.execPath} -e '${failingCodeWithStdoutErr}'`)
    .catch(common.mustCall((err) => {
      assert.strictEqual(err.code, 1);
      assert.strictEqual(err.stdout, 'out\n');
      assert.strictEqual(err.stderr, 'err\n');
    }));
}

{
  execFile(process.execPath, ['-e', failingCodeWithStdoutErr])
    .catch(common.mustCall((err) => {
      assert.strictEqual(err.code, 1);
      assert.strictEqual(err.stdout, 'out\n');
      assert.strictEqual(err.stderr, 'err\n');
    }));
}
