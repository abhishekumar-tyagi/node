'use strict';
const common = require('../common');
const assert = require('assert');
const os = require('os');
const path = require('path');


process.env.TMPDIR = '/tmpdir';
process.env.TMP = '/tmp';
process.env.TEMP = '/temp';
if (common.isWindows) {
  assert.equal(os.tmpdir(), '/temp');
  process.env.TEMP = '';
  assert.equal(os.tmpdir(), '/tmp');
  process.env.TMP = '';
  const expected = (process.env.SystemRoot || process.env.windir) + '\\temp';
  assert.equal(os.tmpdir(), expected);
  process.env.TEMP = '\\temp\\';
  assert.equal(os.tmpdir(), '\\temp');
  process.env.TEMP = '\\tmpdir/';
  assert.equal(os.tmpdir(), '\\tmpdir/');
  process.env.TEMP = '\\';
  assert.equal(os.tmpdir(), '\\');
  process.env.TEMP = 'C:\\';
  assert.equal(os.tmpdir(), 'C:\\');
} else {
  assert.equal(os.tmpdir(), '/tmpdir');
  process.env.TMPDIR = '';
  assert.equal(os.tmpdir(), '/tmp');
  process.env.TMP = '';
  assert.equal(os.tmpdir(), '/temp');
  process.env.TEMP = '';
  assert.equal(os.tmpdir(), '/tmp');
  process.env.TMPDIR = '/tmpdir/';
  assert.equal(os.tmpdir(), '/tmpdir');
  process.env.TMPDIR = '/tmpdir\\';
  assert.equal(os.tmpdir(), '/tmpdir\\');
  process.env.TMPDIR = '/';
  assert.equal(os.tmpdir(), '/');
}

const endianness = os.endianness();
console.log('endianness = %s', endianness);
assert.ok(/[BL]E/.test(endianness));

const hostname = os.hostname();
console.log('hostname = %s', hostname);
assert.ok(hostname.length > 0);

const uptime = os.uptime();
console.log('uptime = %d', uptime);
assert.ok(uptime > 0);

const cpus = os.cpus();
console.log('cpus = ', cpus);
assert.ok(cpus.length > 0);

const type = os.type();
console.log('type = ', type);
assert.ok(type.length > 0);

const release = os.release();
console.log('release = ', release);
assert.ok(release.length > 0);
//TODO: Check format on more than just AIX
if (common.isAix)
  assert.ok(/^\d+\.\d+$/.test(release));

const platform = os.platform();
console.log('platform = ', platform);
assert.ok(platform.length > 0);

const arch = os.arch();
console.log('arch = ', arch);
assert.ok(arch.length > 0);

if (process.platform != 'sunos') {
  // not implemeneted yet
  assert.ok(os.loadavg().length > 0);
  assert.ok(os.freemem() > 0);
  assert.ok(os.totalmem() > 0);
}


const interfaces = os.networkInterfaces();
console.error(interfaces);
switch (platform) {
  case 'linux':
    {
      const filter = function(e) { return e.address == '127.0.0.1'; };
      const actual = interfaces.lo.filter(filter);
      const expected = [{ address: '127.0.0.1', netmask: '255.0.0.0',
        mac: '00:00:00:00:00:00', family: 'IPv4',
        internal: true }];
      assert.deepEqual(actual, expected);
      break;
    }
  case 'win32':
    {
      const filter = function(e) { return e.address == '127.0.0.1'; };
      const actual = interfaces['Loopback Pseudo-Interface 1'].filter(filter);
      const expected = [{ address: '127.0.0.1', netmask: '255.0.0.0',
        mac: '00:00:00:00:00:00', family: 'IPv4',
        internal: true }];
      assert.deepEqual(actual, expected);
      break;
    }
}

const EOL = os.EOL;
assert.ok(EOL.length > 0);


const home = os.homedir();

console.log('homedir = ' + home);
assert.ok(typeof home === 'string');
assert.ok(home.indexOf(path.sep) !== -1);

if (common.isWindows && process.env.USERPROFILE) {
  assert.equal(home, process.env.USERPROFILE);
  delete process.env.USERPROFILE;
  assert.ok(os.homedir().indexOf(path.sep) !== -1);
  process.env.USERPROFILE = home;
} else if (!common.isWindows && process.env.HOME) {
  assert.equal(home, process.env.HOME);
  delete process.env.HOME;
  assert.ok(os.homedir().indexOf(path.sep) !== -1);
  process.env.HOME = home;
}
