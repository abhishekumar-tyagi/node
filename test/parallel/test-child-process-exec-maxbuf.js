'use strict';
const common = require('../common');
const assert = require('assert');
const cp = require('child_process');

function runChecks(err, stdio, streamName, expected) {
  assert.strictEqual(err.message, `${streamName} maxBuffer length exceeded`);
  assert(err instanceof RangeError);
  assert.strictEqual(err.code, 'ERR_CHILD_PROCESS_STDIO_MAXBUFFER');
  assert.deepStrictEqual(stdio[streamName], expected);
}

const env = { NODE: process.execPath };

// default value
{
  const cmd =
    '"$NODE" -e "console.log(\'a\'.repeat(1024 * 1024))"';

  cp.exec(cmd, { env }, common.mustCall((err) => {
    assert(err instanceof RangeError);
    assert.strictEqual(err.message, 'stdout maxBuffer length exceeded');
    assert.strictEqual(err.code, 'ERR_CHILD_PROCESS_STDIO_MAXBUFFER');
  }));
}

// default value
{
  const cmd =
    '"$NODE" -e "console.log(\'a\'.repeat(1024 * 1024 - 1))"';

  cp.exec(cmd, { env }, common.mustSucceed((stdout, stderr) => {
    assert.strictEqual(stdout.trim(), 'a'.repeat(1024 * 1024 - 1));
    assert.strictEqual(stderr, '');
  }));
}

{
  const cmd = '"$NODE" -e "console.log(\'hello world\');"';
  const options = { env, maxBuffer: Infinity };

  cp.exec(cmd, options, common.mustSucceed((stdout, stderr) => {
    assert.strictEqual(stdout.trim(), 'hello world');
    assert.strictEqual(stderr, '');
  }));
}

{
  const cmd = 'echo hello world';

  cp.exec(
    cmd,
    { maxBuffer: 5 },
    common.mustCall((err, stdout, stderr) => {
      runChecks(err, { stdout, stderr }, 'stdout', 'hello');
    })
  );
}

// default value
{
  const cmd =
    '"$NODE" -e "console.log(\'a\'.repeat(1024 * 1024))"';

  cp.exec(
    cmd,
    { env },
    common.mustCall((err, stdout, stderr) => {
      runChecks(
        err,
        { stdout, stderr },
        'stdout',
        'a'.repeat(1024 * 1024)
      );
    })
  );
}

// default value
{
  const cmd =
    '"$NODE" -e "console.log(\'a\'.repeat(1024 * 1024 - 1))"';

  cp.exec(cmd, { env }, common.mustSucceed((stdout, stderr) => {
    assert.strictEqual(stdout.trim(), 'a'.repeat(1024 * 1024 - 1));
    assert.strictEqual(stderr, '');
  }));
}

const unicode = '中文测试'; // length = 4, byte length = 12

{
  const cmd = `"$NODE" -e "console.log('${unicode}');"`;

  cp.exec(
    cmd,
    { env, maxBuffer: 10 },
    common.mustCall((err, stdout, stderr) => {
      runChecks(err, { stdout, stderr }, 'stdout', '中文测试\n');
    })
  );
}

{
  const cmd = `"$NODE" -e "console.error('${unicode}');"`;

  cp.exec(
    cmd,
    { env, maxBuffer: 3 },
    common.mustCall((err, stdout, stderr) => {
      runChecks(err, { stdout, stderr }, 'stderr', '中文测');
    })
  );
}

{
  const cmd = `"$NODE" -e "console.log('${unicode}');"`;

  const child = cp.exec(
    cmd,
    { encoding: null, env, maxBuffer: 10 },
    common.mustCall((err, stdout, stderr) => {
      runChecks(err, { stdout, stderr }, 'stdout', '中文测试\n');
    })
  );

  child.stdout.setEncoding('utf-8');
}

{
  const cmd = `"$NODE" -e "console.error('${unicode}');"`;

  const child = cp.exec(
    cmd,
    { encoding: null, env, maxBuffer: 3 },
    common.mustCall((err, stdout, stderr) => {
      runChecks(err, { stdout, stderr }, 'stderr', '中文测');
    })
  );

  child.stderr.setEncoding('utf-8');
}

{
  const cmd = `"$NODE" -e "console.error('${unicode}');"`;

  cp.exec(
    cmd,
    { encoding: null, env, maxBuffer: 5 },
    common.mustCall((err, stdout, stderr) => {
      const buf = Buffer.from(unicode).slice(0, 5);
      runChecks(err, { stdout, stderr }, 'stderr', buf);
    })
  );
}
