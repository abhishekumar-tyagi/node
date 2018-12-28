'use strict';
const common = require('../common');
common.skipIfInspectorDisabled();
common.requireFlags('--expose-internals');
const { strictEqual } = require('assert');
const { NodeInstance } = require('../common/inspector-helper.js');

async function testNoServerNoCrash() {
  console.log('Test there\'s no crash stopping server that was not started');
  const instance = new NodeInstance([],
                                    `process._debugEnd();
                                     process.exit(42);`);
  strictEqual(42, (await instance.expectShutdown()).exitCode);
}

async function testNoSessionNoCrash() {
  console.log('Test there\'s no crash stopping server without connecting');
  const instance = new NodeInstance('--inspect=0',
                                    'process._debugEnd();process.exit(42);');
  strictEqual(42, (await instance.expectShutdown()).exitCode);
}

async function testSessionNoCrash() {
  console.log('Test there\'s no crash stopping server after connecting');
  const script = `process._debugEnd();
                  process._debugProcess(process.pid);
                  setTimeout(() => {
                      console.log("Done");
                      process.exit(42);
                  });`;

  const instance = new NodeInstance('--inspect-brk=0', script);
  const session = await instance.connectInspectorSession();
  await session.send({ 'method': 'Runtime.runIfWaitingForDebugger' });
  await session.waitForServerDisconnect();
  strictEqual(42, (await instance.expectShutdown()).exitCode);
}

async function runTest() {
  await testNoServerNoCrash();
  await testNoSessionNoCrash();
  await testSessionNoCrash();
}

runTest();
