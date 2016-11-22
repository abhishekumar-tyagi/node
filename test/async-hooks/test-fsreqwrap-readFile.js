'use strict';

const common = require('../common');
const assert = require('assert');
const tick = require('./tick');
const initHooks = require('./init-hooks');
const { checkInvocations } = require('./hook-checks');
const fs = require('fs');

const hooks = initHooks();

hooks.enable();
fs.readFile(__filename, common.mustCall(onread));

function onread() {
  const as = hooks.activitiesOfTypes('FSREQWRAP');
  let lastParent = 1;
  for (let i = 0; i < as.length; i++) {
    const a = as[i];
    assert.strictEqual(a.type, 'FSREQWRAP', 'fs req wrap');
    assert.strictEqual(typeof a.uid, 'number', 'uid is a number');
    assert.strictEqual(a.triggerId, lastParent, 'parent uid 1');
    lastParent = a.uid;
  }
  checkInvocations(as[0], { init: 1, before: 1, after: 1, destroy: 1 },
                   'reqwrap[0]: while in onread callback');
  checkInvocations(as[1], { init: 1, before: 1, after: 1, destroy: 1 },
                   'reqwrap[1]: while in onread callback');
  checkInvocations(as[2], { init: 1, before: 1, after: 1, destroy: 1 },
                   'reqwrap[2]: while in onread callback');

  // this callback is called from within the last fs req callback therefore
  // the last req is still going and after/destroy haven't been called yet
  checkInvocations(as[3], { init: 1, before: 1 },
                   'reqwrap[3]: while in onread callback');
  tick(2);
}

process.on('exit', onexit);

function onexit() {
  hooks.disable();
  hooks.sanityCheck('FSREQWRAP');
  const as = hooks.activitiesOfTypes('FSREQWRAP');
  const a = as.pop();
  checkInvocations(a, { init: 1, before: 1, after: 1, destroy: 1 },
                   'when process exits');
}
