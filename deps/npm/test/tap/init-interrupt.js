/* eslint-disable standard/no-callback-literal */
// if 'npm init' is interrupted with ^C, don't report
// 'init written successfully'
var test = require('tap').test
var npmlog = require('npmlog')
var requireInject = require('require-inject')

var npm = require('../../lib/npm.js')

require('../common-tap.js')

test('issue #6684 remove confusing message', function (t) {
  var initJsonMock = function (dir, input, config, cb) {
    process.nextTick(function () {
      cb({ message: 'canceled' })
    })
  }
  initJsonMock.yes = function () { return true }

  npm.load({ loglevel: 'silent' }, function () {
    var log = ''
    var init = requireInject('../../lib/init', {
      'init-package-json': initJsonMock
    })

    // capture log messages
    npmlog.on('log', function (chunk) { log += chunk.message + '\n' })

    init([], function (err, code) {
      t.ifError(err, 'init ran successfully')
      t.notOk(code, 'exited without issue')
      t.notSimilar(log, /written successfully/, 'no success message written')
      t.similar(log, /canceled/, 'alerted that init was canceled')

      t.end()
    })
  })
})
