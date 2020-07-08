// Copyright Joyent, Inc. and other Node contributors.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so, subject to the
// following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
// NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
// USE OR OTHER DEALINGS IN THE SOFTWARE.

/* eslint-disable node-core/require-common-first, node-core/required-modules */
/* eslint-disable node-core/crypto-check */
'use strict';
const process = global.process;  // Some tests tamper with the process global.

const assert = require('assert');
const { exec, execSync, spawnSync } = require('child_process');
const fs = require('fs');
// Do not require 'os' until needed so that test-os-checked-function can
// monkey patch it. If 'os' is required here, that test will fail.
const path = require('path');
const util = require('util');
const { isMainThread } = require('worker_threads');

const tmpdir = require('./tmpdir');
const bits = ['arm64', 'mips', 'mipsel', 'ppc64', 's390x', 'x64']
  .includes(process.arch) ? 64 : 32;
const hasIntl = !!process.config.variables.v8_enable_i18n_support;

// Some tests assume a umask of 0o022 so set that up front. Tests that need a
// different umask will set it themselves.
//
// Workers can read, but not set the umask, so check that this is the main
// thread.
if (isMainThread)
  process.umask(0o022);

const noop = () => {};

const hasCrypto = Boolean(process.versions.openssl) &&
                  !process.env.NODE_SKIP_CRYPTO;
const hasQuic = hasCrypto && Boolean(process.versions.ngtcp2);

// Check for flags. Skip this for workers (both, the `cluster` module and
// `worker_threads`) and child processes.
// If the binary was built without-ssl then the crypto flags are
// invalid (bad option). The test itself should handle this case.
if (process.argv.length === 2 &&
    !process.env.NODE_SKIP_FLAG_CHECK &&
    isMainThread &&
    hasCrypto &&
    require.main &&
    require('cluster').isMaster) {
  // The copyright notice is relatively big and the flags could come afterwards.
  const bytesToRead = 1500;
  const buffer = Buffer.allocUnsafe(bytesToRead);
  const fd = fs.openSync(require.main.filename, 'r');
  const bytesRead = fs.readSync(fd, buffer, 0, bytesToRead);
  fs.closeSync(fd);
  const source = buffer.toString('utf8', 0, bytesRead);

  const flagStart = source.indexOf('// Flags: --') + 10;
  if (flagStart !== 9) {
    let flagEnd = source.indexOf('\n', flagStart);
    // Normalize different EOL.
    if (source[flagEnd - 1] === '\r') {
      flagEnd--;
    }
    const flags = source
      .substring(flagStart, flagEnd)
      .replace(/_/g, '-')
      .split(' ');
    const args = process.execArgv.map((arg) => arg.replace(/_/g, '-'));
    for (const flag of flags) {
      if (!args.includes(flag) &&
          // If the binary is build without `intl` the inspect option is
          // invalid. The test itself should handle this case.
          (process.features.inspector || !flag.startsWith('--inspect'))) {
        console.log(
          'NOTE: The test started as a child_process using these flags:',
          util.inspect(flags),
          'Use NODE_SKIP_FLAG_CHECK to run the test with the original flags.'
        );
        const args = [...flags, ...process.execArgv, ...process.argv.slice(1)];
        const options = { encoding: 'utf8', stdio: 'inherit' };
        const result = spawnSync(process.execPath, args, options);
        if (result.signal) {
          process.kill(0, result.signal);
        } else {
          process.exit(result.status);
        }
      }
    }
  }
}

const isWindows = process.platform === 'win32';
const isAIX = process.platform === 'aix';
const isSunOS = process.platform === 'sunos';
const isFreeBSD = process.platform === 'freebsd';
const isOpenBSD = process.platform === 'openbsd';
const isLinux = process.platform === 'linux';
const isOSX = process.platform === 'darwin';

const isDumbTerminal = process.env.TERM === 'dumb';

const rootDir = isWindows ? 'c:\\' : '/';

const buildType = process.config.target_defaults ?
  process.config.target_defaults.default_configuration :
  'Release';

// If env var is set then enable async_hook hooks for all tests.
if (process.env.NODE_TEST_WITH_ASYNC_HOOKS) {
  const destroydIdsList = {};
  const destroyListList = {};
  const initHandles = {};
  const { internalBinding } = require('internal/test/binding');
  const async_wrap = internalBinding('async_wrap');

  process.on('exit', () => {
    // Iterate through handles to make sure nothing crashes
    for (const k in initHandles)
      util.inspect(initHandles[k]);
  });

  const _queueDestroyAsyncId = async_wrap.queueDestroyAsyncId;
  async_wrap.queueDestroyAsyncId = function queueDestroyAsyncId(id) {
    if (destroyListList[id] !== undefined) {
      process._rawDebug(destroyListList[id]);
      process._rawDebug();
      throw new Error(`same id added to destroy list twice (${id})`);
    }
    destroyListList[id] = util.inspect(new Error());
    _queueDestroyAsyncId(id);
  };

  require('async_hooks').createHook({
    init(id, ty, tr, resource) {
      if (initHandles[id]) {
        process._rawDebug(
          `Is same resource: ${resource === initHandles[id].resource}`);
        process._rawDebug(`Previous stack:\n${initHandles[id].stack}\n`);
        throw new Error(`init called twice for same id (${id})`);
      }
      initHandles[id] = {
        resource,
        stack: util.inspect(new Error()).substr(6)
      };
    },
    before() { },
    after() { },
    destroy(id) {
      if (destroydIdsList[id] !== undefined) {
        process._rawDebug(destroydIdsList[id]);
        process._rawDebug();
        throw new Error(`destroy called for same id (${id})`);
      }
      destroydIdsList[id] = util.inspect(new Error());
    },
  }).enable();
}

let opensslCli = null;
let inFreeBSDJail = null;
let localhostIPv4 = null;

const localIPv6Hosts =
  isLinux ? [
    // Debian/Ubuntu
    'ip6-localhost',
    'ip6-loopback',

    // SUSE
    'ipv6-localhost',
    'ipv6-loopback',

    // Typically universal
    'localhost',
  ] : [ 'localhost' ];

const PIPE = (() => {
  const localRelative = path.relative(process.cwd(), `${tmpdir.path}/`);
  const pipePrefix = isWindows ? '\\\\.\\pipe\\' : localRelative;
  const pipeName = `node-test.${process.pid}.sock`;
  return path.join(pipePrefix, pipeName);
})();

/*
 * Check that when running a test with
 * `$node --abort-on-uncaught-exception $file child`
 * the process aborts.
 */
function childShouldThrowAndAbort() {
  let testCmd = '';
  if (!isWindows) {
    // Do not create core files, as it can take a lot of disk space on
    // continuous testing and developers' machines
    testCmd += 'ulimit -c 0 && ';
  }
  testCmd += `"${process.argv[0]}" --abort-on-uncaught-exception `;
  testCmd += `"${process.argv[1]}" child`;
  const child = exec(testCmd);
  child.on('exit', function onExit(exitCode, signal) {
    const errMsg = 'Test should have aborted ' +
                   `but instead exited with exit code ${exitCode}` +
                   ` and signal ${signal}`;
    assert(nodeProcessAborted(exitCode, signal), errMsg);
  });
}

function createZeroFilledFile(filename) {
  const fd = fs.openSync(filename, 'w');
  fs.ftruncateSync(fd, 10 * 1024 * 1024);
  fs.closeSync(fd);
}


const pwdCommand = isWindows ?
  ['cmd.exe', ['/d', '/c', 'cd']] :
  ['pwd', []];


function platformTimeout(ms) {
  const multipliers = typeof ms === 'bigint' ?
    { two: 2n, four: 4n, seven: 7n } : { two: 2, four: 4, seven: 7 };

  if (process.features.debug)
    ms = multipliers.two * ms;

  if (isAIX)
    return multipliers.two * ms; // Default localhost speed is slower on AIX

  if (process.arch !== 'arm')
    return ms;

  const armv = process.config.variables.arm_version;

  if (armv === '6')
    return multipliers.seven * ms;  // ARMv6

  if (armv === '7')
    return multipliers.two * ms;  // ARMv7

  return ms; // ARMv8+
}

let knownGlobals = [
  AbortController,
  clearImmediate,
  clearInterval,
  clearTimeout,
  global,
  setImmediate,
  setInterval,
  setTimeout,
  queueMicrotask,
];

if (global.gc) {
  knownGlobals.push(global.gc);
}

function allowGlobals(...whitelist) {
  knownGlobals = knownGlobals.concat(whitelist);
}

if (process.env.NODE_TEST_KNOWN_GLOBALS !== '0') {
  if (process.env.NODE_TEST_KNOWN_GLOBALS) {
    const knownFromEnv = process.env.NODE_TEST_KNOWN_GLOBALS.split(',');
    allowGlobals(...knownFromEnv);
  }

  function leakedGlobals() {
    const leaked = [];

    for (const val in global) {
      if (!knownGlobals.includes(global[val])) {
        leaked.push(val);
      }
    }

    return leaked;
  }

  process.on('exit', function() {
    const leaked = leakedGlobals();
    if (leaked.length > 0) {
      assert.fail(`Unexpected global(s) found: ${leaked.join(', ')}`);
    }
  });
}

const mustCallChecks = [];

function runCallChecks(exitCode) {
  if (exitCode !== 0) return;

  const failed = mustCallChecks.filter(function(context) {
    if ('minimum' in context) {
      context.messageSegment = `at least ${context.minimum}`;
      return context.actual < context.minimum;
    }
    context.messageSegment = `exactly ${context.exact}`;
    return context.actual !== context.exact;
  });

  failed.forEach(function(context) {
    console.log('Mismatched %s function calls. Expected %s, actual %d.',
                context.name,
                context.messageSegment,
                context.actual);
    console.log(context.stack.split('\n').slice(2).join('\n'));
  });

  if (failed.length) process.exit(1);
}

function mustCall(fn, exact) {
  return _mustCallInner(fn, exact, 'exact');
}

function mustCallAtLeast(fn, minimum) {
  return _mustCallInner(fn, minimum, 'minimum');
}

function _mustCallInner(fn, criteria = 1, field) {
  if (process._exiting)
    throw new Error('Cannot use common.mustCall*() in process exit handler');
  if (typeof fn === 'number') {
    criteria = fn;
    fn = noop;
  } else if (fn === undefined) {
    fn = noop;
  }

  if (typeof criteria !== 'number')
    throw new TypeError(`Invalid ${field} value: ${criteria}`);

  const context = {
    [field]: criteria,
    actual: 0,
    stack: util.inspect(new Error()),
    name: fn.name || '<anonymous>'
  };

  // Add the exit listener only once to avoid listener leak warnings
  if (mustCallChecks.length === 0) process.on('exit', runCallChecks);

  mustCallChecks.push(context);

  return function() {
    context.actual++;
    return fn.apply(this, arguments);
  };
}

function hasMultiLocalhost() {
  const { internalBinding } = require('internal/test/binding');
  const { TCP, constants: TCPConstants } = internalBinding('tcp_wrap');
  const t = new TCP(TCPConstants.SOCKET);
  const ret = t.bind('127.0.0.2', 0);
  t.close();
  return ret === 0;
}

function skipIfEslintMissing() {
  if (!fs.existsSync(
    path.join(__dirname, '..', '..', 'tools', 'node_modules', 'eslint')
  )) {
    skip('missing ESLint');
  }
}

function canCreateSymLink() {
  // On Windows, creating symlinks requires admin privileges.
  // We'll only try to run symlink test if we have enough privileges.
  // On other platforms, creating symlinks shouldn't need admin privileges
  if (isWindows) {
    // whoami.exe needs to be the one from System32
    // If unix tools are in the path, they can shadow the one we want,
    // so use the full path while executing whoami
    const whoamiPath = path.join(process.env.SystemRoot,
                                 'System32', 'whoami.exe');

    try {
      const output = execSync(`${whoamiPath} /priv`, { timeout: 1000 });
      return output.includes('SeCreateSymbolicLinkPrivilege');
    } catch {
      return false;
    }
  }
  // On non-Windows platforms, this always returns `true`
  return true;
}

function getCallSite(top) {
  const originalStackFormatter = Error.prepareStackTrace;
  Error.prepareStackTrace = (err, stack) =>
    `${stack[0].getFileName()}:${stack[0].getLineNumber()}`;
  const err = new Error();
  Error.captureStackTrace(err, top);
  // With the V8 Error API, the stack is not formatted until it is accessed
  err.stack;
  Error.prepareStackTrace = originalStackFormatter;
  return err.stack;
}

function mustNotCall(msg) {
  const callSite = getCallSite(mustNotCall);
  return function mustNotCall(...args) {
    const argsInfo = args.length > 0 ?
      `\ncalled with arguments: ${args.map(util.inspect).join(', ')}` : '';
    assert.fail(
      `${msg || 'function should not have been called'} at ${callSite}` +
      argsInfo);
  };
}

function printSkipMessage(msg) {
  console.log(`1..0 # Skipped: ${msg}`);
}

function skip(msg) {
  printSkipMessage(msg);
  process.exit(0);
}

// Returns true if the exit code "exitCode" and/or signal name "signal"
// represent the exit code and/or signal name of a node process that aborted,
// false otherwise.
function nodeProcessAborted(exitCode, signal) {
  // Depending on the compiler used, node will exit with either
  // exit code 132 (SIGILL), 133 (SIGTRAP) or 134 (SIGABRT).
  let expectedExitCodes = [132, 133, 134];

  // On platforms using KSH as the default shell (like SmartOS),
  // when a process aborts, KSH exits with an exit code that is
  // greater than 256, and thus the exit code emitted with the 'exit'
  // event is null and the signal is set to either SIGILL, SIGTRAP,
  // or SIGABRT (depending on the compiler).
  const expectedSignals = ['SIGILL', 'SIGTRAP', 'SIGABRT'];

  // On Windows, 'aborts' are of 2 types, depending on the context:
  // (i) Forced access violation, if --abort-on-uncaught-exception is on
  // which corresponds to exit code 3221225477 (0xC0000005)
  // (ii) Otherwise, _exit(134) which is called in place of abort() due to
  // raising SIGABRT exiting with ambiguous exit code '3' by default
  if (isWindows)
    expectedExitCodes = [0xC0000005, 134];

  // When using --abort-on-uncaught-exception, V8 will use
  // base::OS::Abort to terminate the process.
  // Depending on the compiler used, the shell or other aspects of
  // the platform used to build the node binary, this will actually
  // make V8 exit by aborting or by raising a signal. In any case,
  // one of them (exit code or signal) needs to be set to one of
  // the expected exit codes or signals.
  if (signal !== null) {
    return expectedSignals.includes(signal);
  }
  return expectedExitCodes.includes(exitCode);
}

function isAlive(pid) {
  try {
    process.kill(pid, 'SIGCONT');
    return true;
  } catch {
    return false;
  }
}

function _expectWarning(name, expected, code) {
  if (typeof expected === 'string') {
    expected = [[expected, code]];
  } else if (!Array.isArray(expected)) {
    expected = Object.entries(expected).map(([a, b]) => [b, a]);
  } else if (!(Array.isArray(expected[0]))) {
    expected = [[expected[0], expected[1]]];
  }
  // Deprecation codes are mandatory, everything else is not.
  if (name === 'DeprecationWarning') {
    expected.forEach(([_, code]) => assert(code, expected));
  }
  return mustCall((warning) => {
    const [ message, code ] = expected.shift();
    assert.strictEqual(warning.name, name);
    if (typeof message === 'string') {
      assert.strictEqual(warning.message, message);
    } else {
      assert(message.test(warning.message));
    }
    assert.strictEqual(warning.code, code);
  }, expected.length);
}

let catchWarning;

// Accepts a warning name and description or array of descriptions or a map of
// warning names to description(s) ensures a warning is generated for each
// name/description pair.
// The expected messages have to be unique per `expectWarning()` call.
function expectWarning(nameOrMap, expected, code) {
  if (catchWarning === undefined) {
    catchWarning = {};
    process.on('warning', (warning) => {
      if (!catchWarning[warning.name]) {
        throw new TypeError(
          `"${warning.name}" was triggered without being expected.\n` +
          util.inspect(warning)
        );
      }
      catchWarning[warning.name](warning);
    });
  }
  if (typeof nameOrMap === 'string') {
    catchWarning[nameOrMap] = _expectWarning(nameOrMap, expected, code);
  } else {
    Object.keys(nameOrMap).forEach((name) => {
      catchWarning[name] = _expectWarning(name, nameOrMap[name]);
    });
  }
}

// Useful for testing expected internal/error objects
function expectsError(validator, exact) {
  return mustCall((...args) => {
    if (args.length !== 1) {
      // Do not use `assert.strictEqual()` to prevent `inspect` from
      // always being called.
      assert.fail(`Expected one argument, got ${util.inspect(args)}`);
    }
    const error = args.pop();
    const descriptor = Object.getOwnPropertyDescriptor(error, 'message');
    // The error message should be non-enumerable
    assert.strictEqual(descriptor.enumerable, false);

    assert.throws(() => { throw error; }, validator);
    return true;
  }, exact);
}

function skipIfInspectorDisabled() {
  if (!process.features.inspector) {
    skip('V8 inspector is disabled');
  }
}

function skipIf32Bits() {
  if (bits < 64) {
    skip('The tested feature is not available in 32bit builds');
  }
}

function skipIfWorker() {
  if (!isMainThread) {
    skip('This test only works on a main thread');
  }
}

function getArrayBufferViews(buf) {
  const { buffer, byteOffset, byteLength } = buf;

  const out = [];

  const arrayBufferViews = [
    Int8Array,
    Uint8Array,
    Uint8ClampedArray,
    Int16Array,
    Uint16Array,
    Int32Array,
    Uint32Array,
    Float32Array,
    Float64Array,
    DataView
  ];

  for (const type of arrayBufferViews) {
    const { BYTES_PER_ELEMENT = 1 } = type;
    if (byteLength % BYTES_PER_ELEMENT === 0) {
      out.push(new type(buffer, byteOffset, byteLength / BYTES_PER_ELEMENT));
    }
  }
  return out;
}

function getBufferSources(buf) {
  return [...getArrayBufferViews(buf), new Uint8Array(buf).buffer];
}

// Crash the process on unhandled rejections.
const crashOnUnhandledRejection = (err) => { throw err; };
process.on('unhandledRejection', crashOnUnhandledRejection);
function disableCrashOnUnhandledRejection() {
  process.removeListener('unhandledRejection', crashOnUnhandledRejection);
}

function getTTYfd() {
  // Do our best to grab a tty fd.
  const tty = require('tty');
  // Don't attempt fd 0 as it is not writable on Windows.
  // Ref: ef2861961c3d9e9ed6972e1e84d969683b25cf95
  const ttyFd = [1, 2, 4, 5].find(tty.isatty);
  if (ttyFd === undefined) {
    try {
      return fs.openSync('/dev/tty');
    } catch {
      // There aren't any tty fd's available to use.
      return -1;
    }
  }
  return ttyFd;
}

function runWithInvalidFD(func) {
  let fd = 1 << 30;
  // Get first known bad file descriptor. 1 << 30 is usually unlikely to
  // be an valid one.
  try {
    while (fs.fstatSync(fd--) && fd > 0);
  } catch {
    return func(fd);
  }

  printSkipMessage('Could not generate an invalid fd');
}

// A helper function to simplify checking for ERR_INVALID_ARG_TYPE output.
function invalidArgTypeHelper(input) {
  if (input == null) {
    return ` Received ${input}`;
  }
  if (typeof input === 'function' && input.name) {
    return ` Received function ${input.name}`;
  }
  if (typeof input === 'object') {
    if (input.constructor && input.constructor.name) {
      return ` Received an instance of ${input.constructor.name}`;
    }
    return ` Received ${util.inspect(input, { depth: -1 })}`;
  }
  let inspected = util.inspect(input, { colors: false });
  if (inspected.length > 25)
    inspected = `${inspected.slice(0, 25)}...`;
  return ` Received type ${typeof input} (${inspected})`;
}

function skipIfDumbTerminal() {
  if (isDumbTerminal) {
    skip('skipping - dumb terminal');
  }
}

const common = {
  allowGlobals,
  buildType,
  canCreateSymLink,
  childShouldThrowAndAbort,
  createZeroFilledFile,
  disableCrashOnUnhandledRejection,
  expectsError,
  expectWarning,
  getArrayBufferViews,
  getBufferSources,
  getCallSite,
  getTTYfd,
  hasIntl,
  hasCrypto,
  hasQuic,
  hasMultiLocalhost,
  invalidArgTypeHelper,
  isAIX,
  isAlive,
  isDumbTerminal,
  isFreeBSD,
  isLinux,
  isMainThread,
  isOpenBSD,
  isOSX,
  isSunOS,
  isWindows,
  localIPv6Hosts,
  mustCall,
  mustCallAtLeast,
  mustNotCall,
  nodeProcessAborted,
  PIPE,
  platformTimeout,
  printSkipMessage,
  pwdCommand,
  rootDir,
  runWithInvalidFD,
  skip,
  skipIf32Bits,
  skipIfDumbTerminal,
  skipIfEslintMissing,
  skipIfInspectorDisabled,
  skipIfWorker,

  get enoughTestCpu() {
    const cpus = require('os').cpus();
    return Array.isArray(cpus) && (cpus.length > 1 || cpus[0].speed > 999);
  },

  get enoughTestMem() {
    return require('os').totalmem() > 0x70000000; /* 1.75 Gb */
  },

  get hasFipsCrypto() {
    return hasCrypto && require('crypto').getFips();
  },

  get hasIPv6() {
    const iFaces = require('os').networkInterfaces();
    const re = isWindows ? /Loopback Pseudo-Interface/ : /lo/;
    return Object.keys(iFaces).some((name) => {
      return re.test(name) &&
             iFaces[name].some(({ family }) => family === 'IPv6');
    });
  },

  get inFreeBSDJail() {
    if (inFreeBSDJail !== null) return inFreeBSDJail;

    if (exports.isFreeBSD &&
        execSync('sysctl -n security.jail.jailed').toString() === '1\n') {
      inFreeBSDJail = true;
    } else {
      inFreeBSDJail = false;
    }
    return inFreeBSDJail;
  },

  // On IBMi, process.platform and os.platform() both return 'aix',
  // It is not enough to differentiate between IBMi and real AIX system.
  get isIBMi() {
    return require('os').type() === 'OS400';
  },

  get isLinuxPPCBE() {
    return (process.platform === 'linux') && (process.arch === 'ppc64') &&
           (require('os').endianness() === 'BE');
  },

  get localhostIPv4() {
    if (localhostIPv4 !== null) return localhostIPv4;

    if (this.inFreeBSDJail) {
      // Jailed network interfaces are a bit special - since we need to jump
      // through loops, as well as this being an exception case, assume the
      // user will provide this instead.
      if (process.env.LOCALHOST) {
        localhostIPv4 = process.env.LOCALHOST;
      } else {
        console.error('Looks like we\'re in a FreeBSD Jail. ' +
                      'Please provide your default interface address ' +
                      'as LOCALHOST or expect some tests to fail.');
      }
    }

    if (localhostIPv4 === null) localhostIPv4 = '127.0.0.1';

    return localhostIPv4;
  },

  get localhostIPv6() { return '::1'; },

  // opensslCli defined lazily to reduce overhead of spawnSync
  get opensslCli() {
    if (opensslCli !== null) return opensslCli;

    if (process.config.variables.node_shared_openssl) {
      // Use external command
      opensslCli = 'openssl';
    } else {
      // Use command built from sources included in Node.js repository
      opensslCli = path.join(path.dirname(process.execPath), 'openssl-cli');
    }

    if (exports.isWindows) opensslCli += '.exe';

    const opensslCmd = spawnSync(opensslCli, ['version']);
    if (opensslCmd.status !== 0 || opensslCmd.error !== undefined) {
      // OpenSSL command cannot be executed
      opensslCli = false;
    }
    return opensslCli;
  },

  get PORT() {
    if (+process.env.TEST_PARALLEL) {
      throw new Error('common.PORT cannot be used in a parallelized test');
    }
    return +process.env.NODE_COMMON_PORT || 12346;
  }

};

const validProperties = new Set(Object.keys(common));
module.exports = new Proxy(common, {
  get(obj, prop) {
    if (!validProperties.has(prop))
      throw new Error(`Using invalid common property: '${prop}'`);
    return obj[prop];
  }
});
