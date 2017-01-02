'use strict';

const common = require('../common');
const assert = require('assert');
const fs = require('fs');
const path = require('path');

var openCount = 0;
const _fsopen = fs.open;
const _fsclose = fs.close;

const loopCount = 50;
const totalCheck = 50;
const emptyTxt = path.join(common.fixturesDir, 'empty.txt');

fs.open = function() {
  openCount++;
  return _fsopen.apply(null, arguments);
};

fs.close = function() {
  openCount--;
  return _fsclose.apply(null, arguments);
};

function testLeak(endFn, callback) {
  console.log('testing for leaks from fs.createReadStream().%s()...', endFn);

  var i = 0;
  var check = 0;

  const checkFunction = function() {
    if (openCount !== 0 && check < totalCheck) {
      check++;
      setTimeout(checkFunction, 100);
      return;
    }

    assert.strictEqual(
      0,
      openCount,
      `no leaked file descriptors using ${endFn}() (got ${openCount})`
    );

    openCount = 0;
    callback && setTimeout(callback, 100);
  };

  setInterval(function() {
    const s = fs.createReadStream(emptyTxt);
    s[endFn]();

    if (++i === loopCount) {
      clearTimeout(this);
      setTimeout(checkFunction, 100);
    }
  }, 2);
}

testLeak('close', function() {
  testLeak('destroy');
});
