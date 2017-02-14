var path = require('path')
var fs = require('graceful-fs')
var test = require('tap').test
var mkdirp = require('mkdirp')
var rimraf = require('rimraf')

var common = require('../common-tap.js')

var pkg = path.join(__dirname, 'gently-rm-overeager')
var dep = path.join(__dirname, 'test-whoops')

var EXEC_OPTS = { cwd: pkg }

var fixture = {
  name: '@test/whoops',
  version: '1.0.0',
  scripts: {
    postinstall: 'echo \'nope\' && exit 1'
  }
}

test('setup', function (t) {
  cleanup()
  setup()

  t.end()
})

test('cache add', function (t) {
  common.npm(['install', '../test-whoops'], EXEC_OPTS, function (er, c) {
    t.ifError(er, "test-whoops install didn't explode")
    t.ok(c, 'test-whoops install also failed')
    fs.readdir(pkg, function (er, files) {
      t.ifError(er, 'package directory is still there')
      t.deepEqual(files, [], 'no files remain')
      t.end()
    })
  })
})

test('cleanup', function (t) {
  cleanup()

  t.end()
})

function cleanup () {
  rimraf.sync(pkg)
  rimraf.sync(dep)
}

function setup () {
  mkdirp.sync(pkg)
  // so it doesn't try to install into npm's own node_modules
  mkdirp.sync(path.join(pkg, 'node_modules'))
  mkdirp.sync(dep)
  fs.writeFileSync(path.join(dep, 'package.json'), JSON.stringify(fixture))
}
