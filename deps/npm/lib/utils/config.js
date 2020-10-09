// defaults, types, and shorthands

const {
  typeDefs: {
    semver: { type: semver },
    Umask: { type: Umask },
    url: { type: url },
    path: { type: path }
  }
} = require('@npmcli/config')

const { version: npmVersion } = require('../../package.json')

const ciDetect = require('@npmcli/ci-detect')
const ciName = ciDetect()

const isWindows = require('./is-windows.js')

const editor = process.env.EDITOR ||
  process.env.VISUAL ||
  (isWindows ? 'notepad.exe' : 'vi')

const shell = isWindows ? process.env.ComSpec || 'cmd'
  : process.env.SHELL || 'bash'

const { tmpdir, networkInterfaces } = require('os')
const getLocalAddresses = () => {
  try {
    return Object.values(networkInterfaces()).map(
      int => int.map(({ address }) => address)
    ).reduce((set, addrs) => set.concat(addrs), [undefined])
  } catch (e) {
    return [undefined]
  }
}

const unicode = /UTF-?8$/i.test(
  process.env.LC_ALL || process.env.LC_CTYPE || process.env.LANG
)

// use LOCALAPPDATA on Windows, if set
// https://github.com/npm/cli/pull/899
const cacheRoot = (isWindows && process.env.LOCALAPPDATA) || '~'
const cacheExtra = isWindows ? 'npm-cache' : '.npm'
const cache = `${cacheRoot}/${cacheExtra}`

const defaults = {
  access: null,
  all: false,
  'allow-same-version': false,
  also: null,
  'always-auth': false,
  audit: true,
  'audit-level': null,
  'auth-type': 'legacy',
  before: null,
  'bin-links': true,
  browser: null,
  ca: null,
  cache,
  'cache-lock-retries': 10,
  'cache-lock-stale': 60000,
  'cache-lock-wait': 10000,
  'cache-max': Infinity,
  'cache-min': 10,
  cafile: null,
  call: '',
  cert: null,
  'ci-name': ciName || null,
  cidr: null,
  color: process.env.NO_COLOR == null,
  'commit-hooks': true,
  depth: null,
  description: true,
  dev: false,
  'dry-run': false,
  editor,
  'engine-strict': false,
  'fetch-retries': 2,
  'fetch-retry-factor': 10,
  'fetch-retry-maxtimeout': 60000,
  'fetch-retry-mintimeout': 10000,
  'fetch-timeout': 5 * 60 * 1000,
  force: false,
  'format-package-lock': true,
  fund: true,
  git: 'git',
  'git-tag-version': true,
  global: false,
  'global-style': false,
  // `globalconfig` has its default defined outside of this module
  heading: 'npm',
  'https-proxy': null,
  'if-present': false,
  'ignore-prepublish': false,
  'ignore-scripts': false,
  include: [],
  'include-staged': false,
  'init-author-email': '',
  'init-author-name': '',
  'init-author-url': '',
  'init-license': 'ISC',
  'init-module': '~/.npm-init.js',
  'init-version': '1.0.0',
  json: false,
  key: null,
  'legacy-bundling': false,
  'legacy-peer-deps': false,
  link: false,
  'local-address': undefined,
  loglevel: 'notice',
  'logs-max': 10,
  long: false,
  maxsockets: 50,
  message: '%s',
  'metrics-registry': null,
  'node-options': null,
  'node-version': process.version,
  noproxy: null,
  'npm-version': npmVersion,
  offline: false,
  omit: [],
  only: null,
  optional: true,
  otp: null,
  package: [],
  'package-lock': true,
  'package-lock-only': false,
  parseable: false,
  'prefer-offline': false,
  'prefer-online': false,
  // `prefix` has its default defined outside of this module
  preid: '',
  production: process.env.NODE_ENV === 'production',
  progress: !ciName,
  proxy: null,
  'read-only': false,
  'rebuild-bundle': true,
  registry: 'https://registry.npmjs.org/',
  rollback: true,
  save: true,
  'save-bundle': false,
  'save-dev': false,
  'save-exact': false,
  'save-optional': false,
  'save-prefix': '^',
  'save-prod': false,
  scope: '',
  'script-shell': null,
  'scripts-prepend-node-path': 'warn-only',
  searchexclude: null,
  searchlimit: 20,
  searchopts: '',
  searchstaleness: 15 * 60,
  'send-metrics': false,
  shell,
  shrinkwrap: true,
  'sign-git-commit': false,
  'sign-git-tag': false,
  'sso-poll-frequency': 500,
  'sso-type': 'oauth',
  'strict-peer-deps': false,
  'strict-ssl': true,
  tag: 'latest',
  'tag-version-prefix': 'v',
  timing: false,
  tmp: tmpdir(),
  umask: process.umask ? process.umask() : 0o22,
  unicode,
  'update-notifier': true,
  usage: false,
  'user-agent': 'npm/{npm-version} ' +
                'node/{node-version} ' +
                '{platform} ' +
                '{arch} ' +
                '{ci}',
  userconfig: '~/.npmrc',
  version: false,
  versions: false,
  viewer: isWindows ? 'browser' : 'man'
}

const types = {
  access: [null, 'restricted', 'public'],
  all: Boolean,
  'allow-same-version': Boolean,
  also: [null, 'dev', 'development'],
  'always-auth': Boolean,
  audit: Boolean,
  'audit-level': ['low', 'moderate', 'high', 'critical', 'none', null],
  'auth-type': ['legacy', 'sso', 'saml', 'oauth'],
  before: [null, Date],
  'bin-links': Boolean,
  browser: [null, Boolean, String],
  ca: [null, String, Array],
  cache: path,
  'cache-lock-retries': Number,
  'cache-lock-stale': Number,
  'cache-lock-wait': Number,
  'cache-max': Number,
  'cache-min': Number,
  cafile: path,
  call: String,
  cert: [null, String],
  'ci-name': [null, String],
  cidr: [null, String, Array],
  color: ['always', Boolean],
  'commit-hooks': Boolean,
  depth: [null, Number],
  description: Boolean,
  dev: Boolean,
  'dry-run': Boolean,
  editor: String,
  'engine-strict': Boolean,
  'fetch-retries': Number,
  'fetch-retry-factor': Number,
  'fetch-retry-maxtimeout': Number,
  'fetch-retry-mintimeout': Number,
  'fetch-timeout': Number,
  force: Boolean,
  'format-package-lock': Boolean,
  fund: Boolean,
  git: String,
  'git-tag-version': Boolean,
  global: Boolean,
  'global-style': Boolean,
  globalconfig: path,
  heading: String,
  'https-proxy': [null, url],
  'if-present': Boolean,
  'ignore-prepublish': Boolean,
  'ignore-scripts': Boolean,
  include: [Array, 'prod', 'dev', 'optional', 'peer'],
  'include-staged': Boolean,
  'init-author-email': String,
  'init-author-name': String,
  'init-author-url': ['', url],
  'init-license': String,
  'init-module': path,
  'init-version': semver,
  json: Boolean,
  key: [null, String],
  'legacy-bundling': Boolean,
  'legacy-peer-deps': Boolean,
  link: Boolean,
  'local-address': getLocalAddresses(),
  loglevel: [
    'silent',
    'error',
    'warn',
    'notice',
    'http',
    'timing',
    'info',
    'verbose',
    'silly'
  ],
  'logs-max': Number,
  long: Boolean,
  maxsockets: Number,
  message: String,
  'metrics-registry': [null, String],
  'node-options': [null, String],
  'node-version': [null, semver],
  noproxy: [null, String, Array],
  'npm-version': semver,
  offline: Boolean,
  omit: [Array, 'dev', 'optional', 'peer'],
  only: [null, 'dev', 'development', 'prod', 'production'],
  optional: Boolean,
  otp: [null, String],
  package: [String, Array],
  'package-lock': Boolean,
  'package-lock-only': Boolean,
  parseable: Boolean,
  'prefer-offline': Boolean,
  'prefer-online': Boolean,
  prefix: path,
  preid: String,
  production: Boolean,
  progress: Boolean,
  proxy: [null, false, url], // allow proxy to be disabled explicitly
  'read-only': Boolean,
  'rebuild-bundle': Boolean,
  registry: [null, url],
  rollback: Boolean,
  save: Boolean,
  'save-bundle': Boolean,
  'save-dev': Boolean,
  'save-exact': Boolean,
  'save-optional': Boolean,
  'save-prefix': String,
  'save-prod': Boolean,
  scope: String,
  'script-shell': [null, String],
  'scripts-prepend-node-path': [Boolean, 'auto', 'warn-only'],
  searchexclude: [null, String],
  searchlimit: Number,
  searchopts: String,
  searchstaleness: Number,
  'send-metrics': Boolean,
  shell: String,
  shrinkwrap: Boolean,
  'sign-git-commit': Boolean,
  'sign-git-tag': Boolean,
  'sso-poll-frequency': Number,
  'sso-type': [null, 'oauth', 'saml'],
  'strict-peer-deps': Boolean,
  'strict-ssl': Boolean,
  tag: String,
  'tag-version-prefix': String,
  timing: Boolean,
  tmp: path,
  umask: Umask,
  unicode: Boolean,
  'update-notifier': Boolean,
  usage: Boolean,
  'user-agent': String,
  userconfig: path,
  version: Boolean,
  versions: Boolean,
  viewer: String
}

const shorthands = {
  '?': ['--usage'],
  a: ['--all'],
  B: ['--save-bundle'],
  C: ['--prefix'],
  c: ['--call'],
  D: ['--save-dev'],
  d: ['--loglevel', 'info'],
  dd: ['--loglevel', 'verbose'],
  ddd: ['--loglevel', 'silly'],
  desc: ['--description'],
  E: ['--save-exact'],
  'enjoy-by': ['--before'],
  f: ['--force'],
  g: ['--global'],
  H: ['--usage'],
  h: ['--usage'],
  help: ['--usage'],
  l: ['--long'],
  local: ['--no-global'],
  m: ['--message'],
  n: ['--no-yes'],
  'no-desc': ['--no-description'],
  'no-reg': ['--no-registry'],
  noreg: ['--no-registry'],
  O: ['--save-optional'],
  P: ['--save-prod'],
  p: ['--parseable'],
  porcelain: ['--parseable'],
  q: ['--loglevel', 'warn'],
  quiet: ['--loglevel', 'warn'],
  readonly: ['--read-only'],
  reg: ['--registry'],
  S: ['--save'],
  s: ['--loglevel', 'silent'],
  silent: ['--loglevel', 'silent'],
  v: ['--version'],
  verbose: ['--loglevel', 'verbose'],
  y: ['--yes']
}

module.exports = { defaults, types, shorthands }
