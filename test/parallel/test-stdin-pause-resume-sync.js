'use strict';
require('../common');
console.error('before opening stdin');
process.stdin.resume();
console.error('stdin opened');
console.error('pausing stdin');
process.stdin.pause();
console.error('opening again');
process.stdin.resume();
console.error('pausing again');
process.stdin.pause();
console.error('should exit now');
