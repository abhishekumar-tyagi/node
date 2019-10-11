/* eslint-disable node-core/require-common-first, node-core/required-modules */

import { createRequire } from 'module';

const require = createRequire(import.meta.url);
const common = require('./index.js');

const {
  isMainThread,
  isWindows,
  isAIX,
  isIBMi,
  isLinuxPPCBE,
  isSunOS,
  isFreeBSD,
  isOpenBSD,
  isLinux,
  isOSX,
  enoughTestMem,
  enoughTestCpu,
  rootDir,
  buildType,
  localIPv6Hosts,
  opensslCli,
  PIPE,
  hasIPv6,
  childShouldThrowAndAbort,
  createZeroFilledFile,
  platformTimeout,
  allowGlobals,
  mustCall,
  mustCallAtLeast,
  hasMultiLocalhost,
  skipIfEslintMissing,
  canCreateSymLink,
  getCallSite,
  mustNotCall,
  printSkipMessage,
  skip,
  nodeProcessAborted,
  isAlive,
  expectWarning,
  expectsError,
  skipIfInspectorDisabled,
  skipIf32Bits,
  getArrayBufferViews,
  getBufferSources,
  disableCrashOnUnhandledRejection,
  getTTYfd,
  runWithInvalidFD
} = common;

export {
  isMainThread,
  isWindows,
  isAIX,
  isIBMi,
  isLinuxPPCBE,
  isSunOS,
  isFreeBSD,
  isOpenBSD,
  isLinux,
  isOSX,
  enoughTestMem,
  enoughTestCpu,
  rootDir,
  buildType,
  localIPv6Hosts,
  opensslCli,
  PIPE,
  hasIPv6,
  childShouldThrowAndAbort,
  createZeroFilledFile,
  platformTimeout,
  allowGlobals,
  mustCall,
  mustCallAtLeast,
  hasMultiLocalhost,
  skipIfEslintMissing,
  canCreateSymLink,
  getCallSite,
  mustNotCall,
  printSkipMessage,
  skip,
  nodeProcessAborted,
  isAlive,
  expectWarning,
  expectsError,
  skipIfInspectorDisabled,
  skipIf32Bits,
  getArrayBufferViews,
  getBufferSources,
  disableCrashOnUnhandledRejection,
  getTTYfd,
  runWithInvalidFD,
  createRequire
};
