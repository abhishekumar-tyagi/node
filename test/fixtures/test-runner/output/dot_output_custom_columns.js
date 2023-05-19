// Flags: --test-reporter=dot
'use strict';
process.stdout.columns = 30;

require('../../../common');
const test = require('node:test');
const {setTimeout} = require('timers/promises');

for (let i = 0; i < 100; i++) {
  test(i + ' example', async () => {
    if(i === 0) {
      // So the reporter will run before all tests has started
      await setTimeout(10);
    }
    // resize
    if (i === 28)
      process.stdout.columns = 41;
  });
}
