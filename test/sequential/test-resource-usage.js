var assert = require('assert');
var common = require('../common');


const usage = process.resourceUsage();

assert(usage.userCpuTimeUsedSec >= 0);
assert(usage.userCpuTimeUsedMs >= 0);
assert(usage.systemCpuTimeUsedSec >= 0);
assert(usage.systemCpuTimeUsedMs >= 0);
assert(usage.maxResidentSetSize >= 0);
assert(usage.sharedMemSize >= 0);
assert(usage.integralUnsharedDataSize >= 0);
assert(usage.integralUnsharedStackSize >= 0);
assert(usage.pageReclaims >= 0);
assert(usage.pageFaults >= 0);
assert(usage.swaps >= 0);
assert(usage.blockInputOperations >= 0);
assert(usage.blockOutputOperations >= 0);
assert(usage.ipcMessagesSent >= 0);
assert(usage.ipcMessagesReceived >= 0);
assert(usage.voluntaryContextSwitches >= 0);
assert(usage.involuntaryContextSwitches >= 0);
