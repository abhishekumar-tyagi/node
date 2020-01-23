'use strict'

const BB = require('bluebird')

var fs = require('graceful-fs')
var path = require('path')

var mkdirp = require('mkdirp')
var requireInject = require('require-inject')
var test = require('tap').test

var common = require('../common-tap.js')

var pkg = common.pkg

var json = {
  name: 'bitbucket-https-url-with-creds',
  version: '0.0.0'
}

test('setup', function (t) {
  setup()
  t.end()
})

test('bitbucket-https-url-with-creds', function (t) {
  var cloneUrls = [
    ['https://user:pass@bitbucket.org/foo/private.git', 'Bitbucket URLs with passwords try only that.']
  ]

  var npm = requireInject.installGlobally('../../lib/npm.js', {
    'pacote/lib/util/git': {
      'revs': (repo, opts) => {
        return BB.resolve().then(() => {
          var cloneUrl = cloneUrls.shift()
          if (cloneUrl) {
            t.is(repo, cloneUrl[0], cloneUrl[1])
          } else {
            t.fail('too many attempts to clone')
          }
          throw new Error('git.revs mock fails on purpose')
        })
      }
    }
  })

  var opts = {
    cache: common.cache,
    prefix: pkg,
    registry: common.registry,
    loglevel: 'silent'
  }
  npm.load(opts, function (er) {
    t.ifError(er, 'npm loaded without error')
    npm.commands.install(['git+https://user:pass@bitbucket.org/foo/private.git'], function (err) {
      t.match(err, /mock fails on purpose/, 'mocked install failed as expected')
      t.end()
    })
  })
})

function setup () {
  mkdirp.sync(pkg)
  fs.writeFileSync(
    path.join(pkg, 'package.json'),
    JSON.stringify(json, null, 2)
  )
  process.chdir(pkg)
}
