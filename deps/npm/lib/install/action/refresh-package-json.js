'use strict'
const path = require('path')
const Bluebird = require('bluebird')
const readJson = Bluebird.promisify(require('read-package-json'))
const updatePackageJson = Bluebird.promisify(require('../update-package-json'))
const getRequested = require('../get-requested.js')

module.exports = function (staging, pkg, log) {
  log.silly('refresh-package-json', pkg.realpath)

  return readJson(path.join(pkg.path, 'package.json'), false).then((metadata) => {
    Object.keys(pkg.package).forEach(function (key) {
      if (key !== '_injectedFromShrinkwrap' && !isEmpty(pkg.package[key])) {
        metadata[key] = pkg.package[key]
        if (key === '_resolved' && metadata[key] == null && pkg.package._injectedFromShrinkwrap) {
          metadata[key] = pkg.package._injectedFromShrinkwrap.resolved
        }
      }
    })
    // These two sneak in and it's awful
    delete metadata.readme
    delete metadata.readmeFilename

    pkg.package = metadata
  }).catch(() => 'ignore').then(() => {
    const requested = pkg.package._requested || getRequested(pkg)
    if (requested.type !== 'directory') {
      return updatePackageJson(pkg, pkg.path)
    }
  })
}

function isEmpty (value) {
  if (value == null) return true
  if (Array.isArray(value)) return !value.length
  if (typeof value === 'object') return !Object.keys(value).length
  return false
}

