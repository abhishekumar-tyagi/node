const t = require('tap')
const ansiTrim = require('../../../lib/utils/ansi-trim.js')
const chalk = require('chalk')
t.equal(ansiTrim('foo'), 'foo', 'does nothing if no ansis')
t.equal(ansiTrim(chalk.red('foo')), 'foo', 'strips out ansis')
