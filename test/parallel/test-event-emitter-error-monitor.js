'use strict';
const common = require('../common');
const assert = require('assert');
const EventEmitter = require('events');

const EE = new EventEmitter();
const theErr = new Error('MyError');

EE.on(
  EventEmitter.errorMonitorSymbol,
  common.mustCall((e) => assert.strictEqual(e, theErr), 3)
);

// Verify with no error listener
common.expectsError(
  () => EE.emit('error', theErr), theErr
);

// Verify with error listener
EE.once('error', common.mustCall((e) => assert.strictEqual(e, theErr)));
EE.emit('error', theErr);


// Verify it works with once
process.nextTick(() => EE.emit('error', theErr));
async function testOnce() {
  try {
    await EventEmitter.once(EE, 'notTriggered');
  } catch (e) {
    assert.strictEqual(e, theErr);
  }
}
testOnce();

// Only error events trigger error monitor
EE.on('aEvent', common.mustCall());
EE.emit('aEvent');
