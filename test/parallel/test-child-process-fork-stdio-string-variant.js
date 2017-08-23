'use strict';
const common = require('../common');

// Ensures that child_process.fork can accept string
// variant of stdio parameter in options object and
// throws a TypeError when given an unexpected string

const assert = require('assert');
const fork = require('child_process').fork;
const fixtures = require('../common/fixtures');

const childScript = fixtures.path('child-process-spawn-node');
const malFormedOpts = { stdio: '33' };
const payload = { hello: 'world' };

const expectedError =
  common.expectsError({ code: 'ERR_INVALID_OPT_VALUE', type: TypeError });

assert.throws(() => fork(childScript, malFormedOpts), expectedError);

function test(stringVariant) {
  const child = fork(childScript, { stdio: stringVariant });

  child.on('message', common.mustCall((message) => {
    assert.deepStrictEqual(message, { foo: 'bar' });
  }));

  child.send(payload);

  child.on('exit', common.mustCall((code) => assert.strictEqual(code, 0)));
}

['pipe', 'inherit', 'ignore'].forEach(test);
