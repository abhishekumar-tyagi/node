// print a banner telling the user to upgrade npm to latest
// but not in CI, and not if we're doing that already.
// Check daily for betas, and weekly otherwise.

const ciInfo = require('ci-info')
const semver = require('semver')
const { stat, writeFile } = require('fs/promises')
const { resolve } = require('path')

// update check frequency
const DAILY = 1000 * 60 * 60 * 24
const WEEKLY = DAILY * 7

// don't put it in the _cacache folder, just in npm's cache
const lastCheckedFile = npm =>
  resolve(npm.flatOptions.cache, '../_update-notifier-last-checked')

// Actual check for updates. This is a separate function so that we only load
// this if we are doing the actual update
const updateCheck = async (npm, spec, version, current) => {
  const pacote = require('pacote')

  const mani = await pacote.manifest(`npm@${spec}`, {
    // always prefer latest, even if doing --tag=whatever on the cmd
    defaultTag: 'latest',
    ...npm.flatOptions,
    cache: false,
  }).catch(() => null)

  // if pacote failed, give up
  if (!mani) {
    return null
  }

  const latest = mani.version

  // if the current version is *greater* than latest, we're on a 'next'
  // and should get the updates from that release train.
  // Note that this isn't another http request over the network, because
  // the packument will be cached by pacote from previous request.
  if (semver.gt(version, latest) && spec === 'latest') {
    return updateNotifier(npm, `^${version}`)
  }

  // if we already have something >= the desired spec, then we're done
  if (semver.gte(version, latest)) {
    return null
  }

  const useColor = npm.logColor
  const chalk = npm.logChalk

  // ok!  notify the user about this update they should get.
  // The message is saved for printing at process exit so it will not get
  // lost in any other messages being printed as part of the command.
  const update = semver.parse(mani.version)
  const type = update.major !== current.major ? 'major'
    : update.minor !== current.minor ? 'minor'
    : update.patch !== current.patch ? 'patch'
    : 'prerelease'
  const typec = type === 'major' ? chalk.red(type)
    : type === 'minor' ? chalk.yellow(type)
    : chalk.green(type)
  const oldc = chalk.red(current)
  const latestc = chalk.green(latest)
  const changelog = `https://github.com/npm/cli/releases/tag/v${latest}`
  const changelogc = !useColor ? `<${changelog}>` : chalk.cyan(changelog)
  const cmd = `npm install -g npm@${latest}`
  const cmdc = !useColor ? `\`${cmd}\`` : chalk.green(cmd)
  const message = `\nNew ${typec} version of npm available! ` +
    `${oldc} -> ${latestc}\n` +
    `Changelog: ${changelogc}\n` +
    `Run ${cmdc} to update!\n`

  return message
}

const updateNotifier = async (npm, spec = 'latest') => {
  // if we're on a prerelease train, then updates are coming fast
  // check for a new one daily.  otherwise, weekly.
  const { version } = npm
  const current = semver.parse(version)

  // if we're on a beta train, always get the next beta
  if (current.prerelease.length) {
    spec = `^${version}`
  }

  // while on a beta train, get updates daily
  const duration = spec !== 'latest' ? DAILY : WEEKLY

  const t = new Date(Date.now() - duration)
  // if we don't have a file, then definitely check it.
  const st = await stat(lastCheckedFile(npm)).catch(() => ({ mtime: t - 1 }))

  // if we've already checked within the specified duration, don't check again
  if (!(t > st.mtime)) {
    return null
  }

  // intentional.  do not await this.  it's a best-effort update.  if this
  // fails, it's ok.  might be using /dev/null as the cache or something weird
  // like that.
  writeFile(lastCheckedFile(npm), '').catch(() => {})

  return updateCheck(npm, spec, version, current)
}

// only update the notification timeout if we actually finished checking
module.exports = npm => {
  if (
    // opted out
    !npm.config.get('update-notifier')
    // global npm update
    || (npm.flatOptions.global &&
      ['install', 'update'].includes(npm.command) &&
      npm.argv.some(arg => /^npm(@|$)/.test(arg)))
    // CI
    || ciInfo.isCI
  ) {
    return Promise.resolve(null)
  }

  return updateNotifier(npm)
}
