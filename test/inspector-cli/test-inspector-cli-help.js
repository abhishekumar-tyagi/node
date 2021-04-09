'use strict';
const common = require('../common');

common.skipIfInspectorDisabled();

const fixtures = require('../common/fixtures');
const startCLI = require('../common/inspector-cli');

const assert = require('assert');

{
  const cli = startCLI([fixtures.path('inspector-cli/empty.js')]);

  function onFatal(error) {
    cli.quit();
    throw error;
  }

  return cli.waitForInitialBreak()
    .then(() => cli.waitForPrompt())
    .then(() => cli.command('help'))
    .then(() => {
      assert.match(cli.output, /run, restart, r\s+/m);
    })
    .then(() => cli.quit())
    .then(null, onFatal);
}
