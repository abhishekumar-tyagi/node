// Flags: --experimental-permission --allow-fs-read=* --allow-child-process
'use strict';

const common = require('../common');
common.skipIfWorker();
common.skipIfInspectorDisabled();

const { Session } = require('inspector');
const assert = require('assert');
const { spawnSync } = require('child_process');

if (!common.hasCrypto)
  common.skip('no crypto');

{
  assert.throws(() => {
    const session = new Session();
    session.connect();
  }, common.expectsError({
    code: 'ERR_ACCESS_DENIED',
    permission: 'Inspector',
  }));
}

{
  const { status, stderr } = spawnSync(
    process.execPath,
    [
      '--experimental-permission',
      '-e',
      '(new (require("inspector")).Session()).connect()',
    ],
  );
  assert.strictEqual(status, 1);
  assert.match(stderr.toString(), /Error: Access to this API has been restricted/);
}
