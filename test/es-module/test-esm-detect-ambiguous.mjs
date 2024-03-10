import { spawnPromisified } from '../common/index.mjs';
import * as fixtures from '../common/fixtures.mjs';
import { spawn } from 'node:child_process';
import { describe, it } from 'node:test';
import { strictEqual, match } from 'node:assert';

describe('--experimental-detect-module', { concurrency: true }, () => {
  describe('string input', { concurrency: true }, () => {
    it('permits ESM syntax in --eval input without requiring --input-type=module', async () => {
      const { stdout, stderr, code, signal } = await spawnPromisified(process.execPath, [
        '--experimental-detect-module',
        '--eval',
        'import { version } from "node:process"; console.log(version);',
      ]);

      strictEqual(stderr, '');
      strictEqual(stdout, `${process.version}\n`);
      strictEqual(code, 0);
      strictEqual(signal, null);
    });

    // ESM is unsupported for --print via --input-type=module

    it('permits ESM syntax in STDIN input without requiring --input-type=module', async () => {
      const child = spawn(process.execPath, [
        '--experimental-detect-module',
      ]);
      child.stdin.end('console.log(typeof import.meta.resolve)');

      match((await child.stdout.toArray()).toString(), /^function\r?\n$/);
    });

    it('should be overridden by --input-type', async () => {
      const { code, signal, stdout, stderr } = await spawnPromisified(process.execPath, [
        '--experimental-detect-module',
        '--input-type=commonjs',
        '--eval',
        'import.meta.url',
      ]);

      match(stderr, /SyntaxError: Cannot use 'import\.meta' outside a module/);
      strictEqual(stdout, '');
      strictEqual(code, 1);
      strictEqual(signal, null);
    });

    it('should be overridden by --experimental-default-type', async () => {
      const { code, signal, stdout, stderr } = await spawnPromisified(process.execPath, [
        '--experimental-detect-module',
        '--experimental-default-type=commonjs',
        '--eval',
        'import.meta.url',
      ]);

      match(stderr, /SyntaxError: Cannot use 'import\.meta' outside a module/);
      strictEqual(stdout, '');
      strictEqual(code, 1);
      strictEqual(signal, null);
    });

    it('does not trigger detection via source code `eval()`', async () => {
      const { code, signal, stdout, stderr } = await spawnPromisified(process.execPath, [
        '--experimental-detect-module',
        '--eval',
        'eval("import \'nonexistent\';");',
      ]);

      match(stderr, /SyntaxError: Cannot use import statement outside a module/);
      strictEqual(stdout, '');
      strictEqual(code, 1);
      strictEqual(signal, null);
    });
  });

  describe('.js file input in a typeless package', { concurrency: true }, () => {
    for (const { testName, entryPath } of [
      {
        testName: 'permits CommonJS syntax in a .js entry point',
        entryPath: fixtures.path('es-modules/package-without-type/commonjs.js'),
      },
      {
        testName: 'permits ESM syntax in a .js entry point',
        entryPath: fixtures.path('es-modules/package-without-type/module.js'),
      },
      {
        testName: 'permits CommonJS syntax in a .js file imported by a CommonJS entry point',
        entryPath: fixtures.path('es-modules/package-without-type/imports-commonjs.cjs'),
      },
      {
        testName: 'permits ESM syntax in a .js file imported by a CommonJS entry point',
        entryPath: fixtures.path('es-modules/package-without-type/imports-esm.js'),
      },
      {
        testName: 'permits CommonJS syntax in a .js file imported by an ESM entry point',
        entryPath: fixtures.path('es-modules/package-without-type/imports-commonjs.mjs'),
      },
      {
        testName: 'permits ESM syntax in a .js file imported by an ESM entry point',
        entryPath: fixtures.path('es-modules/package-without-type/imports-esm.mjs'),
      },
    ]) {
      it(testName, async () => {
        const { stdout, stderr, code, signal } = await spawnPromisified(process.execPath, [
          '--experimental-detect-module',
          entryPath,
        ]);

        strictEqual(stderr, '');
        strictEqual(stdout, 'executed\n');
        strictEqual(code, 0);
        strictEqual(signal, null);
      });
    }
  });

  describe('extensionless file input in a typeless package', { concurrency: true }, () => {
    for (const { testName, entryPath } of [
      {
        testName: 'permits CommonJS syntax in an extensionless entry point',
        entryPath: fixtures.path('es-modules/package-without-type/noext-cjs'),
      },
      {
        testName: 'permits ESM syntax in an extensionless entry point',
        entryPath: fixtures.path('es-modules/package-without-type/noext-esm'),
      },
      {
        testName: 'permits CommonJS syntax in an extensionless file imported by a CommonJS entry point',
        entryPath: fixtures.path('es-modules/package-without-type/imports-noext-cjs.js'),
      },
      {
        testName: 'permits ESM syntax in an extensionless file imported by a CommonJS entry point',
        entryPath: fixtures.path('es-modules/package-without-type/imports-noext-esm.js'),
      },
      {
        testName: 'permits CommonJS syntax in an extensionless file imported by an ESM entry point',
        entryPath: fixtures.path('es-modules/package-without-type/imports-noext-cjs.mjs'),
      },
      {
        testName: 'permits ESM syntax in an extensionless file imported by an ESM entry point',
        entryPath: fixtures.path('es-modules/package-without-type/imports-noext-esm.mjs'),
      },
    ]) {
      it(testName, async () => {
        const { stdout, stderr, code, signal } = await spawnPromisified(process.execPath, [
          '--experimental-detect-module',
          entryPath,
        ]);

        strictEqual(stderr, '');
        strictEqual(stdout, 'executed\n');
        strictEqual(code, 0);
        strictEqual(signal, null);
      });
    }

    it('should not hint wrong format in resolve hook', async () => {
      let writeSync;
      const { stdout, stderr, code, signal } = await spawnPromisified(process.execPath, [
        '--experimental-detect-module',
        '--no-warnings',
        '--loader',
        `data:text/javascript,import { writeSync } from "node:fs"; export ${encodeURIComponent(
          async function resolve(s, c, next) {
            const result = await next(s, c);
            writeSync(1, result.format + '\n');
            return result;
          }
        )}`,
        fixtures.path('es-modules/package-without-type/noext-esm'),
      ]);

      strictEqual(stderr, '');
      strictEqual(stdout, 'null\nexecuted\n');
      strictEqual(code, 0);
      strictEqual(signal, null);

    });
  });

  describe('file input in a "type": "commonjs" package', { concurrency: true }, () => {
    for (const { testName, entryPath } of [
      {
        testName: 'disallows ESM syntax in a .js entry point',
        entryPath: fixtures.path('es-modules/package-type-commonjs/module.js'),
      },
      {
        testName: 'disallows ESM syntax in a .js file imported by a CommonJS entry point',
        entryPath: fixtures.path('es-modules/package-type-commonjs/imports-esm.js'),
      },
      {
        testName: 'disallows ESM syntax in a .js file imported by an ESM entry point',
        entryPath: fixtures.path('es-modules/package-type-commonjs/imports-esm.mjs'),
      },
    ]) {
      it(testName, async () => {
        const { stdout, stderr, code, signal } = await spawnPromisified(process.execPath, [
          '--experimental-detect-module',
          entryPath,
        ]);

        match(stderr, /SyntaxError: Unexpected token 'export'/);
        strictEqual(stdout, '');
        strictEqual(code, 1);
        strictEqual(signal, null);
      });
    }
  });

  describe('file input in a "type": "module" package', { concurrency: true }, () => {
    for (const { testName, entryPath } of [
      {
        testName: 'disallows CommonJS syntax in a .js entry point',
        entryPath: fixtures.path('es-modules/package-type-module/cjs.js'),
      },
      {
        testName: 'disallows CommonJS syntax in a .js file imported by a CommonJS entry point',
        entryPath: fixtures.path('es-modules/package-type-module/imports-commonjs.cjs'),
      },
      {
        testName: 'disallows CommonJS syntax in a .js file imported by an ESM entry point',
        entryPath: fixtures.path('es-modules/package-type-module/imports-commonjs.mjs'),
      },
    ]) {
      it(testName, async () => {
        const { stdout, stderr, code, signal } = await spawnPromisified(process.execPath, [
          '--experimental-detect-module',
          entryPath,
        ]);

        match(stderr, /ReferenceError: module is not defined in ES module scope/);
        strictEqual(stdout, '');
        strictEqual(code, 1);
        strictEqual(signal, null);
      });
    }
  });

  // https://github.com/nodejs/node/issues/50917
  describe('syntax that errors in CommonJS but works in ESM', { concurrency: true }, () => {
    it('permits top-level `await`', async () => {
      const { stdout, stderr, code, signal } = await spawnPromisified(process.execPath, [
        '--experimental-detect-module',
        '--eval',
        'await Promise.resolve(); console.log("executed");',
      ]);

      strictEqual(stderr, '');
      strictEqual(stdout, 'executed\n');
      strictEqual(code, 0);
      strictEqual(signal, null);
    });

    it('permits top-level `await` above import/export syntax', async () => {
      const { stdout, stderr, code, signal } = await spawnPromisified(process.execPath, [
        '--experimental-detect-module',
        '--eval',
        'await Promise.resolve(); import "node:os"; console.log("executed");',
      ]);

      strictEqual(stderr, '');
      strictEqual(stdout, 'executed\n');
      strictEqual(code, 0);
      strictEqual(signal, null);
    });

    it('still throws on `await` in an ordinary sync function', async () => {
      const { stdout, stderr, code, signal } = await spawnPromisified(process.execPath, [
        '--experimental-detect-module',
        '--eval',
        'function fn() { await Promise.resolve(); } fn();',
      ]);

      match(stderr, /SyntaxError: await is only valid in async function/);
      strictEqual(stdout, '');
      strictEqual(code, 1);
      strictEqual(signal, null);
    });

    it('permits declaration of CommonJS module variables', async () => {
      const { stdout, stderr, code, signal } = await spawnPromisified(process.execPath, [
        '--experimental-detect-module',
        fixtures.path('es-modules/package-without-type/commonjs-wrapper-variables.js'),
      ]);

      strictEqual(stderr, '');
      strictEqual(stdout, 'exports require module __filename __dirname\n');
      strictEqual(code, 0);
      strictEqual(signal, null);
    });

    it('permits declaration of CommonJS module variables above import/export', async () => {
      const { stdout, stderr, code, signal } = await spawnPromisified(process.execPath, [
        '--experimental-detect-module',
        '--eval',
        'const module = 3; import "node:os"; console.log("executed");',
      ]);

      strictEqual(stderr, '');
      strictEqual(stdout, 'executed\n');
      strictEqual(code, 0);
      strictEqual(signal, null);
    });
  });
});

// Validate temporarily disabling `--abort-on-uncaught-exception`
// while running `containsModuleSyntax`.
// Ref: https://github.com/nodejs/node/issues/50878
describe('Wrapping a `require` of an ES module while using `--abort-on-uncaught-exception`', () => {
  it('should work', async () => {
    const { code, signal, stdout, stderr } = await spawnPromisified(process.execPath, [
      '--abort-on-uncaught-exception',
      '--eval',
      'assert.throws(() => require("./package-type-module/esm.js"), { code: "ERR_REQUIRE_ESM" })',
    ], {
      cwd: fixtures.path('es-modules'),
    });

    strictEqual(stderr, '');
    strictEqual(stdout, '');
    strictEqual(code, 0);
    strictEqual(signal, null);
  });
});
