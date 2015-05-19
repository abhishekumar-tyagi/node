'use strict';
var assert = require('assert'),
    fs = require('fs'),
    saneEmitter,
    sanity = 'ire(\'assert\')';

saneEmitter = fs.createReadStream(__filename, { start: 31, end: 43 });

assert.throws(function() {
  fs.createReadStream(__filename, { start: '31', end: 43 });
}, "start as string didn't throw an error for createReadStream");

assert.throws(function() {
  fs.createReadStream(__filename, { start: 31, end: '43' });
}, "end as string didn't throw an error");

assert.throws(function() {
  fs.createWriteStream(__filename, { start: '31' });
}, "start as string didn't throw an error for createWriteStream");

saneEmitter.on('data', function(data) {
  // a sanity check when using numbers instead of strings
  assert.strictEqual(sanity, data.toString('utf8'), 'read ' +
                     data.toString('utf8') + ' instead of ' + sanity);
});
