/* IMPORTANT
 * This snapshot file is auto-generated, but designed for humans.
 * It should be checked into source control and tracked carefully.
 * Re-generate by setting TAP_SNAPSHOT=1 and running tests.
 * Make sure to inspect the output below.  Do not ignore changes!
 */
'use strict'
exports[`test/lib/utils/explain-dep.js TAP > ellipses test one 1`] = `
manydep@1.0.0
  manydep@"1.0.0" from prod-dep@1.2.3
  node_modules/prod-dep
    prod-dep@"1.x" from the root project
  6 more (optdep, extra-neos, deep-dev, peer, ...)
`

exports[`test/lib/utils/explain-dep.js TAP > ellipses test two 1`] = `
manydep@1.0.0
  manydep@"1.0.0" from prod-dep@1.2.3
  node_modules/prod-dep
    prod-dep@"1.x" from the root project
  5 more (optdep, extra-neos, deep-dev, peer, a package with a pretty long name)
`

exports[`test/lib/utils/explain-dep.js TAP deepDev > explain color deep 1`] = `
[1mdeep-dev[22m@[1m2.3.4[22m [1m[33mdev[39m[22m[2m[22m
[2mnode_modules/deep-dev[22m
  [1mdeep-dev[22m@"[1m2.x[22m" from [1mmetadev[22m@[1m3.4.5[22m[2m[22m
  [2mnode_modules/dev/node_modules/metadev[22m
    [1mmetadev[22m@"[1m3.x[22m" from [1mtopdev[22m@[1m4.5.6[22m[2m[22m
    [2mnode_modules/topdev[22m
      [33mdev[39m [1mtopdev[22m@"[1m4.x[22m" from the root project
`

exports[`test/lib/utils/explain-dep.js TAP deepDev > explain nocolor shallow 1`] = `
deep-dev@2.3.4 dev
node_modules/deep-dev
  deep-dev@"2.x" from metadev@3.4.5
  node_modules/dev/node_modules/metadev
    metadev@"3.x" from topdev@4.5.6
    node_modules/topdev
`

exports[`test/lib/utils/explain-dep.js TAP deepDev > print color 1`] = `
[1mdeep-dev[22m@[1m2.3.4[22m [1m[33mdev[39m[22m[2m[22m
[2mnode_modules/deep-dev[22m
`

exports[`test/lib/utils/explain-dep.js TAP deepDev > print nocolor 1`] = `
deep-dev@2.3.4 dev
node_modules/deep-dev
`

exports[`test/lib/utils/explain-dep.js TAP extraneous > explain color deep 1`] = `
[1mextra-neos[22m@[1m1337.420.69-lol[22m [1m[31mextraneous[39m[22m[2m[22m
[2mnode_modules/extra-neos[22m
`

exports[`test/lib/utils/explain-dep.js TAP extraneous > explain nocolor shallow 1`] = `
extra-neos@1337.420.69-lol extraneous
node_modules/extra-neos
`

exports[`test/lib/utils/explain-dep.js TAP extraneous > print color 1`] = `
[1mextra-neos[22m@[1m1337.420.69-lol[22m [1m[31mextraneous[39m[22m[2m[22m
[2mnode_modules/extra-neos[22m
`

exports[`test/lib/utils/explain-dep.js TAP extraneous > print nocolor 1`] = `
extra-neos@1337.420.69-lol extraneous
node_modules/extra-neos
`

exports[`test/lib/utils/explain-dep.js TAP manyDeps > explain color deep 1`] = `
[1mmanydep[22m@[1m1.0.0[22m
  [1mmanydep[22m@"[1m1.0.0[22m" from [1mprod-dep[22m@[1m1.2.3[22m[2m[22m
  [2mnode_modules/prod-dep[22m
    [1mprod-dep[22m@"[1m1.x[22m" from the root project
  [36moptional[39m [1mmanydep[22m@"[1m1.x[22m" from [1moptdep[22m@[1m1.0.0[22m [1m[36moptional[39m[22m[2m[22m
  [2mnode_modules/optdep[22m
    optdep [1moptdep[22m@"[1m1.0.0[22m" from the root project
  [1mmanydep[22m@"[1m1.0.x[22m" from [1mextra-neos[22m@[1m1337.420.69-lol[22m [1m[31mextraneous[39m[22m[2m[22m
  [2mnode_modules/extra-neos[22m
  [33mdev[39m [1mmanydep[22m@"[1m*[22m" from [1mdeep-dev[22m@[1m2.3.4[22m [1m[33mdev[39m[22m[2m[22m
  [2mnode_modules/deep-dev[22m
    [1mdeep-dev[22m@"[1m2.x[22m" from [1mmetadev[22m@[1m3.4.5[22m[2m[22m
    [2mnode_modules/dev/node_modules/metadev[22m
      [1mmetadev[22m@"[1m3.x[22m" from [1mtopdev[22m@[1m4.5.6[22m[2m[22m
      [2mnode_modules/topdev[22m
        [33mdev[39m [1mtopdev[22m@"[1m4.x[22m" from the root project
  [35mpeer[39m [1mmanydep[22m@"[1m>1.0.0-beta <1.0.1[22m" from [1mpeer[22m@[1m1.0.0[22m [1m[35mpeer[39m[22m[2m[22m
  [2mnode_modules/peer[22m
    [35mpeer[39m [1mpeer[22m@"[1m1.0.0[22m" from the root project
  [1mmanydep[22m@"[1m1[22m" from [1ma package with a pretty long name[22m@[1m1.2.3[22m
  [1mmanydep[22m@"[1m1[22m" from [1manother package with a pretty long name[22m@[1m1.2.3[22m
  [1mmanydep[22m@"[1m1[22m" from [1myet another a package with a pretty long name[22m@[1m1.2.3[22m
`

exports[`test/lib/utils/explain-dep.js TAP manyDeps > explain nocolor shallow 1`] = `
manydep@1.0.0
  manydep@"1.0.0" from prod-dep@1.2.3
  node_modules/prod-dep
    prod-dep@"1.x" from the root project
  7 more (optdep, extra-neos, deep-dev, peer, ...)
`

exports[`test/lib/utils/explain-dep.js TAP manyDeps > print color 1`] = `
[1mmanydep[22m@[1m1.0.0[22m
`

exports[`test/lib/utils/explain-dep.js TAP manyDeps > print nocolor 1`] = `
manydep@1.0.0
`

exports[`test/lib/utils/explain-dep.js TAP optional > explain color deep 1`] = `
[1moptdep[22m@[1m1.0.0[22m [1m[36moptional[39m[22m[2m[22m
[2mnode_modules/optdep[22m
  optdep [1moptdep[22m@"[1m1.0.0[22m" from the root project
`

exports[`test/lib/utils/explain-dep.js TAP optional > explain nocolor shallow 1`] = `
optdep@1.0.0 optional
node_modules/optdep
  optdep optdep@"1.0.0" from the root project
`

exports[`test/lib/utils/explain-dep.js TAP optional > print color 1`] = `
[1moptdep[22m@[1m1.0.0[22m [1m[36moptional[39m[22m[2m[22m
[2mnode_modules/optdep[22m
`

exports[`test/lib/utils/explain-dep.js TAP optional > print nocolor 1`] = `
optdep@1.0.0 optional
node_modules/optdep
`

exports[`test/lib/utils/explain-dep.js TAP peer > explain color deep 1`] = `
[1mpeer[22m@[1m1.0.0[22m [1m[35mpeer[39m[22m[2m[22m
[2mnode_modules/peer[22m
  [35mpeer[39m [1mpeer[22m@"[1m1.0.0[22m" from the root project
`

exports[`test/lib/utils/explain-dep.js TAP peer > explain nocolor shallow 1`] = `
peer@1.0.0 peer
node_modules/peer
  peer peer@"1.0.0" from the root project
`

exports[`test/lib/utils/explain-dep.js TAP peer > print color 1`] = `
[1mpeer[22m@[1m1.0.0[22m [1m[35mpeer[39m[22m[2m[22m
[2mnode_modules/peer[22m
`

exports[`test/lib/utils/explain-dep.js TAP peer > print nocolor 1`] = `
peer@1.0.0 peer
node_modules/peer
`

exports[`test/lib/utils/explain-dep.js TAP prodDep > explain color deep 1`] = `
[1mprod-dep[22m@[1m1.2.3[22m[2m[22m
[2mnode_modules/prod-dep[22m
  [1mprod-dep[22m@"[1m1.x[22m" from the root project
`

exports[`test/lib/utils/explain-dep.js TAP prodDep > explain nocolor shallow 1`] = `
prod-dep@1.2.3
node_modules/prod-dep
  prod-dep@"1.x" from the root project
`

exports[`test/lib/utils/explain-dep.js TAP prodDep > print color 1`] = `
[1mprod-dep[22m@[1m1.2.3[22m[2m[22m
[2mnode_modules/prod-dep[22m
`

exports[`test/lib/utils/explain-dep.js TAP prodDep > print nocolor 1`] = `
prod-dep@1.2.3
node_modules/prod-dep
`
