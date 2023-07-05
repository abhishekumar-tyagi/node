'use strict';

// Flags: --expose-internals

require('../common');

const { describe, it } = require('node:test');
const path = require('node:path');
const assert = require('node:assert');
const { pathToFileURL } = require('node:url');
const { legacyMainResolve } = require('node:internal/modules/esm/resolve');

const fixtures = require('../common/fixtures.js');

describe('legacyMainResolve', () => {
  it('should resolve using packageConfig.main', () => {
    const packageJsonUrl = pathToFileURL(
      path.resolve(
        fixtures.path('/es-modules/legacy-main-resolver'),
        'package.json'
      )
    );

    const paths = [
      ['./index-js/index.js', './index-js/index.js'],
      ['./index-js/index', './index-js/index.js'],
      ['./index-json/index', './index-json/index.json'],
      ['./index-node/index', './index-node/index.node'],
      ['./index-js', './index-js/index.js'],
      ['./index-json', './index-json/index.json'],
      ['./index-node', './index-node/index.node'],
    ];

    for (const [main, expected] of paths) {
      const packageConfig = { main };
      const base = path.resolve(
        fixtures.path('/es-modules/legacy-main-resolver')
      );

      assert.strictEqual(
        legacyMainResolve(packageJsonUrl, packageConfig, base).href,
        pathToFileURL(path.join(base, expected)).href
      );
    }
  });

  it('should resolve using packageJsonUrl', () => {
    const paths = [
      ['index-js', './index-js/index.js'],
      ['index-json', './index-json/index.json'],
      ['index-node', './index-node/index.node'],
    ];

    for (const [folder, expected] of paths) {
      const packageJsonUrl = pathToFileURL(
        path.resolve(
          fixtures.path('/es-modules/legacy-main-resolver'),
          folder,
          'package.json'
        )
      );
      const packageConfig = { main: undefined };
      const base = path.resolve(
        fixtures.path('/es-modules/legacy-main-resolver')
      );

      assert.strictEqual(
        legacyMainResolve(packageJsonUrl, packageConfig, base).href,
        pathToFileURL(path.join(base, expected)).href
      );
    }
  });

  it('should throw when packageJsonUrl is not URL', () => {
    assert.throws(
      () =>
        legacyMainResolve(
          path.resolve(
            fixtures.path('/es-modules/legacy-main-resolver/index-node'),
            'package.json'
          ),
          {},
          ''
        ),
      { message: /instance of URL/ }
    );
  });

  it('should throw when packageConfigMain is invalid URL', () => {
    assert.throws(
      () =>
        legacyMainResolve(
          pathToFileURL(
            path.resolve(
              // Is invalid because this path does not point to a file
              fixtures.path('/es-modules/legacy-main-resolver/index-node')
            )
          ),
          { main: './invalid/index.js' },
          ''
        ),
      { message: /Invalid URL/ }
    );
  });

  it('should throw when packageJsonUrl is invalid URL', () => {
    assert.throws(
      () =>
        legacyMainResolve(
          pathToFileURL(
            path.resolve(
              // Is invalid because this path does not point to a file
              fixtures.path('/es-modules/legacy-main-resolver/index-node')
            )
          ),
          { main: undefined },
          ''
        ),
      { message: /Invalid URL/ }
    );
  });

  it('should throw when cannot resolve to a file', () => {
    const packageJsonUrl = pathToFileURL(
      path.resolve(
        fixtures.path('/es-modules/legacy-main-resolver'),
        'package.json'
      )
    );
    assert.throws(
      () => legacyMainResolve(packageJsonUrl, { main: null }, packageJsonUrl),
      { message: /Cannot find package/i }
    );
  });

  it('should throw when cannot resolve to a file (base not defined)', () => {
    const packageJsonUrl = pathToFileURL(
      path.resolve(
        fixtures.path('/es-modules/legacy-main-resolver'),
        'package.json'
      )
    );
    assert.throws(
      () => legacyMainResolve(packageJsonUrl, { main: null }, undefined),
      { message: /"base" argument must be/ }
    );
  });
});
