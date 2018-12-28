'use strict';

// This tests that fs.access and fs.accessSync works as expected
// and the errors thrown from these APIs include the desired properties

const common = require('../common');

common.requireFlags('--expose-internals');

const assert = require('assert');
const fs = require('fs');
const path = require('path');

const { internalBinding } = require('internal/test/binding');
const { UV_ENOENT } = internalBinding('uv');

const tmpdir = require('../common/tmpdir');
const doesNotExist = path.join(tmpdir.path, '__this_should_not_exist');
const readOnlyFile = path.join(tmpdir.path, 'read_only_file');
const readWriteFile = path.join(tmpdir.path, 'read_write_file');

function createFileWithPerms(file, mode) {
  fs.writeFileSync(file, '');
  fs.chmodSync(file, mode);
}

tmpdir.refresh();
createFileWithPerms(readOnlyFile, 0o444);
createFileWithPerms(readWriteFile, 0o666);

/*
 * On non-Windows supported platforms, fs.access(readOnlyFile, W_OK, ...)
 * always succeeds if node runs as the super user, which is sometimes the
 * case for tests running on our continuous testing platform agents.
 *
 * In this case, this test tries to change its process user id to a
 * non-superuser user so that the test that checks for write access to a
 * read-only file can be more meaningful.
 *
 * The change of user id is done after creating the fixtures files for the same
 * reason: the test may be run as the superuser within a directory in which
 * only the superuser can create files, and thus it may need superuser
 * privileges to create them.
 *
 * There's not really any point in resetting the process' user id to 0 after
 * changing it to 'nobody', since in the case that the test runs without
 * superuser privilege, it is not possible to change its process user id to
 * superuser.
 *
 * It can prevent the test from removing files created before the change of user
 * id, but that's fine. In this case, it is the responsibility of the
 * continuous integration platform to take care of that.
 */
let hasWriteAccessForReadonlyFile = false;
if (!common.isWindows && process.getuid() === 0) {
  hasWriteAccessForReadonlyFile = true;
  try {
    process.setuid('nobody');
    hasWriteAccessForReadonlyFile = false;
  } catch {
  }
}

assert.strictEqual(typeof fs.F_OK, 'number');
assert.strictEqual(typeof fs.R_OK, 'number');
assert.strictEqual(typeof fs.W_OK, 'number');
assert.strictEqual(typeof fs.X_OK, 'number');

const throwNextTick = (e) => { process.nextTick(() => { throw e; }); };

fs.access(__filename, common.mustCall(function(...args) {
  assert.deepStrictEqual(args, [null]);
}));
fs.promises.access(__filename)
  .then(common.mustCall())
  .catch(throwNextTick);
fs.access(__filename, fs.R_OK, common.mustCall(function(...args) {
  assert.deepStrictEqual(args, [null]);
}));
fs.promises.access(__filename, fs.R_OK)
  .then(common.mustCall())
  .catch(throwNextTick);
fs.access(readOnlyFile, fs.F_OK | fs.R_OK, common.mustCall(function(...args) {
  assert.deepStrictEqual(args, [null]);
}));
fs.promises.access(readOnlyFile, fs.F_OK | fs.R_OK)
  .then(common.mustCall())
  .catch(throwNextTick);

{
  const expectedError = (err) => {
    assert.notStrictEqual(err, null);
    assert.strictEqual(err.code, 'ENOENT');
    assert.strictEqual(err.path, doesNotExist);
  };
  fs.access(doesNotExist, common.mustCall(expectedError));
  fs.promises.access(doesNotExist)
    .then(common.mustNotCall(), common.mustCall(expectedError))
    .catch(throwNextTick);
}

{
  function expectedError(err) {
    assert.strictEqual(this, undefined);
    if (hasWriteAccessForReadonlyFile) {
      assert.ifError(err);
    } else {
      assert.notStrictEqual(err, null);
      assert.strictEqual(err.path, readOnlyFile);
    }
  }
  fs.access(readOnlyFile, fs.W_OK, common.mustCall(expectedError));
  fs.promises.access(readOnlyFile, fs.W_OK)
    .then(common.mustNotCall(), common.mustCall(expectedError))
    .catch(throwNextTick);
}

{
  const expectedError = (err) => {
    assert.strictEqual(err.code, 'ERR_INVALID_ARG_TYPE');
    assert.ok(err instanceof TypeError);
    return true;
  };
  assert.throws(
    () => { fs.access(100, fs.F_OK, common.mustNotCall()); },
    expectedError
  );

  fs.promises.access(100, fs.F_OK)
    .then(common.mustNotCall(), common.mustCall(expectedError))
    .catch(throwNextTick);
}

common.expectsError(
  () => {
    fs.access(__filename, fs.F_OK);
  },
  {
    code: 'ERR_INVALID_CALLBACK',
    type: TypeError
  });

common.expectsError(
  () => {
    fs.access(__filename, fs.F_OK, {});
  },
  {
    code: 'ERR_INVALID_CALLBACK',
    type: TypeError
  });

// Regular access should not throw.
fs.accessSync(__filename);
const mode = fs.F_OK | fs.R_OK | fs.W_OK;
fs.accessSync(readWriteFile, mode);

assert.throws(
  () => { fs.accessSync(doesNotExist); },
  (err) => {
    assert.strictEqual(err.code, 'ENOENT');
    assert.strictEqual(err.path, doesNotExist);
    assert.strictEqual(
      err.message,
      `ENOENT: no such file or directory, access '${doesNotExist}'`
    );
    assert.strictEqual(err.constructor, Error);
    assert.strictEqual(err.syscall, 'access');
    assert.strictEqual(err.errno, UV_ENOENT);
    return true;
  }
);

assert.throws(
  () => { fs.accessSync(Buffer.from(doesNotExist)); },
  (err) => {
    assert.strictEqual(err.code, 'ENOENT');
    assert.strictEqual(err.path, doesNotExist);
    assert.strictEqual(
      err.message,
      `ENOENT: no such file or directory, access '${doesNotExist}'`
    );
    assert.strictEqual(err.constructor, Error);
    assert.strictEqual(err.syscall, 'access');
    assert.strictEqual(err.errno, UV_ENOENT);
    return true;
  }
);
