// Flags: --expose-internals
'use strict';

const common = require('../common');

// The following tests assert that the node.cc PrintHelp() function
// returns the proper set of cli options when invoked

const assert = require('assert');
const { exec } = require('child_process');
let stdOut;


function startPrintHelpTest() {
  exec(`${process.execPath} --help`, common.mustSucceed((stdout, stderr) => {
    stdOut = stdout;
    validateNodePrintHelp();
  }));
}

function validateNodePrintHelp() {
  const HAVE_OPENSSL = common.hasCrypto;
  const NODE_HAVE_I18N_SUPPORT = common.hasIntl;
  const HAVE_INSPECTOR = process.features.inspector;

  const cliHelpOptions = [
    { compileConstant: HAVE_OPENSSL,
      flags: [ '--openssl-config=...', '--tls-cipher-list=...',
               '--use-bundled-ca', '--use-openssl-ca',
               '--enable-fips', '--force-fips' ] },
    { compileConstant: NODE_HAVE_I18N_SUPPORT,
      flags: [ '--icu-data-dir=...', 'NODE_ICU_DATA' ] },
    { compileConstant: HAVE_INSPECTOR,
      flags: [ '--inspect-brk[=[host:]port]', '--inspect-port=[host:]port',
               '--inspect[=[host:]port]' ] },
  ];

  cliHelpOptions.forEach(testForSubstring);
}

function testForSubstring(options) {
  if (options.compileConstant) {
    options.flags.forEach((flag) => {
      assert.strictEqual(stdOut.indexOf(flag) !== -1, true,
                         `Missing flag ${flag} in ${stdOut}`);
    });
  } else {
    options.flags.forEach((flag) => {
      assert.strictEqual(stdOut.indexOf(flag), -1,
                         `Unexpected flag ${flag} in ${stdOut}`);
    });
  }
}

startPrintHelpTest();

// Test closed stdout for `node --help`. Like `node --help | head -n5`.
{
  const socket = new net.Socket();
  socket.end();
  const result = spawnSync(process.execPath, ['--help'], {
    stdio: ['inherit', socket, 'inherit']
  });
  assert.strictEqual(child.status, 0, 'node --help should exit with code 0');
  assert(!child.error, 'node --help should not have an error');
  socket.destroy();
}
