const path = require('node:path');
const url = require('node:url');
const os = require('node:os');
const fs = require('node:fs');

const tmpfile = path.join(os.tmpdir(), 'file');
fs.writeFileSync(tmpfile, '');

process.send({ 'watch:require': [path.resolve(__filename)] });
process.send({ 'watch:import': [url.pathToFileURL(path.resolve(__filename)).toString()] });
process.send({ 'watch:import': [url.pathToFileURL(tmpfile).toString()] });
process.send({ 'watch:import': [new URL('http://invalid.com').toString()] });
