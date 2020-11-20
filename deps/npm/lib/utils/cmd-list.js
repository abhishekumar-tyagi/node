// short names for common things
const shorthands = {
  un: 'uninstall',
  rb: 'rebuild',
  list: 'ls',
  ln: 'link',
  create: 'init',
  i: 'install',
  it: 'install-test',
  cit: 'install-ci-test',
  up: 'update',
  c: 'config',
  s: 'search',
  se: 'search',
  tst: 'test',
  t: 'test',
  ddp: 'dedupe',
  v: 'view',
  run: 'run-script',
  'clean-install': 'ci',
  'clean-install-test': 'cit',
  x: 'exec',
  why: 'explain',
}

const affordances = {
  la: 'll',
  verison: 'version',
  ic: 'ci',
  innit: 'init',
  // manually abbrev so that install-test doesn't make insta stop working
  in: 'install',
  ins: 'install',
  inst: 'install',
  insta: 'install',
  instal: 'install',
  isnt: 'install',
  isnta: 'install',
  isntal: 'install',
  'install-clean': 'ci',
  'isntall-clean': 'ci',
  hlep: 'help',
  'dist-tags': 'dist-tag',
  upgrade: 'update',
  udpate: 'update',
  login: 'adduser',
  'add-user': 'adduser',
  author: 'owner',
  home: 'docs',
  issues: 'bugs',
  info: 'view',
  show: 'view',
  find: 'search',
  add: 'install',
  unlink: 'uninstall',
  remove: 'uninstall',
  rm: 'uninstall',
  r: 'uninstall',
  rum: 'run-script',
  sit: 'cit',
  urn: 'run-script',
  ogr: 'org',
}

// these are filenames in .
const cmdList = [
  'ci',
  'install-ci-test',
  'install',
  'install-test',
  'uninstall',
  'cache',
  'config',
  'set',
  'get',
  'update',
  'outdated',
  'prune',
  'pack',
  'find-dupes',
  'dedupe',
  'hook',

  'rebuild',
  'link',

  'publish',
  'star',
  'stars',
  'unstar',
  'adduser',
  'login', // This is an alias for `adduser` but it can be confusing
  'logout',
  'unpublish',
  'owner',
  'access',
  'team',
  'deprecate',
  'shrinkwrap',
  'token',
  'profile',
  'audit',
  'fund',
  'org',

  'help',
  'ls',
  'll',
  'search',
  'view',
  'init',
  'version',
  'edit',
  'explore',
  'docs',
  'repo',
  'bugs',
  'root',
  'prefix',
  'bin',
  'whoami',
  'dist-tag',
  'ping',

  'test',
  'stop',
  'start',
  'restart',
  'run-script',
  'completion',
  'doctor',
  'exec',
  'explain',
]

const plumbing = ['birthday', 'help-search']
module.exports = {
  aliases: Object.assign({}, shorthands, affordances),
  shorthands,
  affordances,
  cmdList,
  plumbing,
}
