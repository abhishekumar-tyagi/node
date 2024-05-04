/* eslint-disable @stylistic/js/max-len */

import { createRequire } from 'node:module';
import { fileURLToPath, URL } from 'node:url';

function requireEslintPlugin(specifier) {
  const require = createRequire(import.meta.url);
  try {
    return require(`./tools/node_modules/${specifier}`);
  } catch {
    return require(
      `./tools/node_modules/eslint/node_modules/${specifier}`);
  }
}

const js = requireEslintPlugin('@eslint/js');
const babelEslintParser = requireEslintPlugin('@babel/eslint-parser');
const babelPluginSyntaxImportAttributes = requireEslintPlugin('@babel/plugin-syntax-import-attributes');
const jsdoc = requireEslintPlugin('eslint-plugin-jsdoc');
const markdown = requireEslintPlugin('eslint-plugin-markdown');
const stylisticJs = requireEslintPlugin('@stylistic/eslint-plugin-js');
const globals = requireEslintPlugin('globals');

const nodeCore = requireEslintPlugin('eslint-plugin-node-core');
nodeCore.RULES_DIR = fileURLToPath(new URL('./tools/eslint-rules', import.meta.url));

const restrictedSyntaxCommon = [
  {
    selector: "CallExpression[callee.name='setTimeout'][arguments.length<2]",
    message: '`setTimeout()` must be invoked with at least two arguments.',
  },
  {
    selector: "CallExpression[callee.name='setInterval'][arguments.length<2]",
    message: '`setInterval()` must be invoked with at least two arguments.',
  },
  {
    selector: 'ThrowStatement > CallExpression[callee.name=/Error$/]',
    message: 'Use `new` keyword when throwing an `Error`.',
  },
  {
    selector: "CallExpression[callee.name='isNaN']",
    message: 'Use Number.isNaN() instead of the global isNaN() function.',
  },
];

export default [
  // #region general
  js.configs.recommended,
  jsdoc.configs['flat/recommended'],
  {
    ignores: [
      '**/node_modules/**',
      'lib/punycode.js',
      'test/addons/??_*',
      'test/fixtures/**',
      'test/message/esm_display_syntax_error.mjs',
      'tools/icu/**',
      'tools/lint-md/lint-md.mjs',
      'tools/github_reporter/**',
      'benchmark/tmp/**',
      'benchmark/fixtures/**',
      'doc/**/*.js',
      'doc/changelogs/CHANGELOG_v1*.md',
      '!doc/changelogs/CHANGELOG_v18.md',
      '!doc/api_assets/*.js',
    ],
  },
  {
    files: ['**/*.js'],
    languageOptions: {
      sourceType: 'commonjs',
    },
  },
  {
    plugins: {
      jsdoc,
      '@stylistic/js': stylisticJs,
      'node-core': nodeCore,
    },
    languageOptions: {
      parser: babelEslintParser,
      parserOptions: {
        babelOptions: {
          plugins: [
            babelPluginSyntaxImportAttributes,
          ],
        },
        requireConfigFile: false,
        sourceType: 'script',
      },
      globals: {
        ByteLengthQueuingStrategy: 'readable',
        CompressionStream: 'readable',
        CountQueuingStrategy: 'readable',
        CustomEvent: 'readable',
        crypto: 'readable',
        Crypto: 'readable',
        CryptoKey: 'readable',
        DecompressionStream: 'readable',
        fetch: 'readable',
        FormData: 'readable',
        navigator: 'readable',
        ReadableStream: 'readable',
        ReadableStreamDefaultReader: 'readable',
        ReadableStreamBYOBReader: 'readable',
        ReadableStreamBYOBRequest: 'readable',
        ReadableByteStreamController: 'readable',
        ReadableStreamDefaultController: 'readable',
        Response: 'readable',
        TextDecoderStream: 'readable',
        TextEncoderStream: 'readable',
        TransformStream: 'readable',
        TransformStreamDefaultController: 'readable',
        ShadowRealm: 'readable',
        SubtleCrypto: 'readable',
        WritableStream: 'readable',
        WritableStreamDefaultWriter: 'readable',
        WritableStreamDefaultController: 'readable',
        WebSocket: 'readable',
      },
    },
    rules: {
      // ESLint built-in rules
      // https://eslint.org/docs/rules/
      'accessor-pairs': 'error',
      'array-callback-return': 'error',
      'block-scoped-var': 'error',
      'capitalized-comments': ['error', 'always', {
        line: {
          // Ignore all lines that have less characters than 20 and all lines that
          // start with something that looks like a variable name or code.
          ignorePattern: '.{0,20}$|[a-z]+ ?[0-9A-Z_.(/=:[#-]|std|http|ssh|ftp',
          ignoreInlineComments: true,
          ignoreConsecutiveComments: true,
        },
        block: {
          ignorePattern: '.*',
        },
      }],
      'default-case-last': 'error',
      'dot-notation': 'error',
      'eqeqeq': ['error', 'smart'],
      'func-name-matching': 'error',
      'func-style': ['error', 'declaration', { allowArrowFunctions: true }],
      'no-constant-condition': ['error', { checkLoops: false }],
      'no-constructor-return': 'error',
      'no-duplicate-imports': 'error',
      'no-else-return': 'error',
      'no-lonely-if': 'error',
      'no-mixed-requires': 'error',
      'no-new-require': 'error',
      'no-path-concat': 'error',
      'no-proto': 'error',
      'no-redeclare': ['error', { 'builtinGlobals': false }],
      'no-restricted-modules': ['error', 'sys'],
      'no-restricted-properties': [
        'error',
        {
          object: 'assert',
          property: 'deepEqual',
          message: 'Use `assert.deepStrictEqual()`.',
        },
        {
          object: 'assert',
          property: 'notDeepEqual',
          message: 'Use `assert.notDeepStrictEqual()`.',
        },
        {
          object: 'assert',
          property: 'equal',
          message: 'Use `assert.strictEqual()` rather than `assert.equal()`.',
        },
        {
          object: 'assert',
          property: 'notEqual',
          message: 'Use `assert.notStrictEqual()` rather than `assert.notEqual()`.',
        },
        {
          property: '__defineGetter__',
          message: '__defineGetter__ is deprecated.',
        },
        {
          property: '__defineSetter__',
          message: '__defineSetter__ is deprecated.',
        },
      ],
      'no-restricted-syntax': [
        'error',
        ...restrictedSyntaxCommon,
        {
          // TODO(@panva): move this to no-restricted-properties
          // when https://github.com/eslint/eslint/issues/16412 is fixed
          selector: "Identifier[name='webcrypto']",
          message: 'Use `globalThis.crypto`.',
        },
      ],
      'no-self-compare': 'error',
      'no-template-curly-in-string': 'error',
      'no-throw-literal': 'error',
      'no-undef': ['error', { typeof: true }],
      'no-undef-init': 'error',
      'no-unused-expressions': ['error', { allowShortCircuit: true }],
      'no-unused-vars': ['error', { args: 'none', caughtErrors: 'all' }],
      'no-use-before-define': ['error', {
        classes: true,
        functions: false,
        variables: false,
      }],
      'no-useless-call': 'error',
      'no-useless-concat': 'error',
      'no-useless-constructor': 'error',
      'no-useless-return': 'error',
      'no-var': 'error',
      'no-void': 'error',
      'one-var': ['error', { initialized: 'never' }],
      'prefer-const': ['error', { ignoreReadBeforeAssign: true }],
      'prefer-object-has-own': 'error',
      'strict': ['error', 'global'],
      'symbol-description': 'error',
      'unicode-bom': 'error',
      'valid-typeof': ['error', { requireStringLiterals: true }],

      // ESLint recommended rules that we disable
      'no-inner-declarations': 'off',

      // JSDoc recommended rules that we disable
      'jsdoc/require-jsdoc': 'off',
      'jsdoc/require-param-description': 'off',
      'jsdoc/newline-after-description': 'off',
      'jsdoc/require-returns-description': 'off',
      'jsdoc/valid-types': 'off',
      'jsdoc/no-defaults': 'off',
      'jsdoc/no-undefined-types': 'off',
      'jsdoc/require-param': 'off',
      'jsdoc/check-tag-names': 'off',
      'jsdoc/require-returns': 'off',

      // Stylistic rules
      '@stylistic/js/arrow-parens': 'error',
      '@stylistic/js/arrow-spacing': 'error',
      '@stylistic/js/block-spacing': 'error',
      '@stylistic/js/brace-style': ['error', '1tbs', { allowSingleLine: true }],
      '@stylistic/js/comma-dangle': ['error', 'always-multiline'],
      '@stylistic/js/comma-spacing': 'error',
      '@stylistic/js/comma-style': 'error',
      '@stylistic/js/computed-property-spacing': 'error',
      '@stylistic/js/dot-location': ['error', 'property'],
      '@stylistic/js/eol-last': 'error',
      '@stylistic/js/func-call-spacing': 'error',
      '@stylistic/js/indent': ['error', 2, {
        ArrayExpression: 'first',
        CallExpression: { arguments: 'first' },
        FunctionDeclaration: { parameters: 'first' },
        FunctionExpression: { parameters: 'first' },
        MemberExpression: 'off',
        ObjectExpression: 'first',
        SwitchCase: 1,
      }],
      '@stylistic/js/key-spacing': 'error',
      '@stylistic/js/keyword-spacing': 'error',
      '@stylistic/js/linebreak-style': 'error',
      '@stylistic/js/max-len': ['error', {
        code: 120,
        ignorePattern: '^// Flags:',
        ignoreRegExpLiterals: true,
        ignoreTemplateLiterals: true,
        ignoreUrls: true,
        tabWidth: 2,
      }],
      '@stylistic/js/new-parens': 'error',
      '@stylistic/js/no-confusing-arrow': 'error',
      '@stylistic/js/no-extra-parens': ['error', 'functions'],
      '@stylistic/js/no-multi-spaces': ['error', { ignoreEOLComments: true }],
      '@stylistic/js/no-multiple-empty-lines': ['error', { max: 2, maxEOF: 0, maxBOF: 0 }],
      '@stylistic/js/no-tabs': 'error',
      '@stylistic/js/no-trailing-spaces': 'error',
      '@stylistic/js/no-whitespace-before-property': 'error',
      '@stylistic/js/object-curly-newline': 'error',
      '@stylistic/js/object-curly-spacing': ['error', 'always'],
      '@stylistic/js/one-var-declaration-per-line': 'error',
      '@stylistic/js/operator-linebreak': ['error', 'after'],
      '@stylistic/js/padding-line-between-statements': [
        'error',
        { blankLine: 'always', prev: 'function', next: 'function' },
      ],
      '@stylistic/js/quotes': ['error', 'single', { avoidEscape: true }],
      '@stylistic/js/quote-props': ['error', 'consistent'],
      '@stylistic/js/rest-spread-spacing': 'error',
      '@stylistic/js/semi': 'error',
      '@stylistic/js/semi-spacing': 'error',
      '@stylistic/js/space-before-blocks': ['error', 'always'],
      '@stylistic/js/space-before-function-paren': ['error', {
        anonymous: 'never',
        named: 'never',
        asyncArrow: 'always',
      }],
      '@stylistic/js/space-in-parens': 'error',
      '@stylistic/js/space-infix-ops': 'error',
      '@stylistic/js/space-unary-ops': 'error',
      '@stylistic/js/spaced-comment': ['error', 'always', {
        'block': { 'balanced': true },
        'exceptions': ['-'],
      }],
      '@stylistic/js/template-curly-spacing': 'error',

      // Custom rules
      'node-core/no-unescaped-regexp-dot': 'error',
      'node-core/no-duplicate-requires': 'error',
      'node-core/prefer-proto': 'error',
    },
  },
  // #endregion
  // #region benchmark
  {
    files: ['benchmark/**/*.{js,mjs,cjs}'],
    languageOptions: {
      globals: {
        ...globals.node,
      },
    },
    rules: {
      'prefer-arrow-callback': 'error',
    },
  },
  // #endregion
  // #region doc
  {
    files: ['**/*.md'],
    plugins: {
      markdown,
    },
    processor: 'markdown/markdown',
  },
  {
    files: ['doc/**/*.md/*.{js,mjs,cjs}'],
    rules: {
      // Ease some restrictions in doc examples
      'no-restricted-properties': 'off',
      'no-undef': 'off',
      'no-unused-expressions': 'off',
      'no-unused-vars': 'off',
      'symbol-description': 'off',

      // Add new ECMAScript features gradually
      'prefer-const': 'error',
      'prefer-rest-params': 'error',
      'prefer-template': 'error',

      // Stylistic rules
      '@stylistic/js/no-multiple-empty-lines': [
        'error',
        {
          max: 1,
          maxEOF: 0,
          maxBOF: 0,
        },
      ],
    },
  },
  {
    files: ['**/*.md/*.{js,cjs}'],
    languageOptions: {
      parserOptions: {
        sourceType: 'script',
        ecmaFeatures: { impliedStrict: true },
      },
    },
    rules: { strict: 'off' },
  },
  {
    files: [
      '**/*.md/*.mjs',
      'doc/api/esm.md/*.js',
      'doc/api/packages.md/*.js',
    ],
    languageOptions: {
      parserOptions: { sourceType: 'module' },
    },
    rules: { 'no-restricted-globals': [
      'error',
      {
        name: '__filename',
        message: 'Use import.meta.url instead',
      },
      {
        name: '__dirname',
        message: 'Not available in ESM',
      },
      {
        name: 'exports',
        message: 'Not available in ESM',
      },
      {
        name: 'module',
        message: 'Not available in ESM',
      },
      {
        name: 'require',
        message: 'Use import instead',
      },
      {
        name: 'Buffer',
        message: 'Import Buffer instead of using the global',
      },
      {
        name: 'process',
        message: 'Import process instead of using the global',
      },
    ] },
  },
  {
    files: ['doc/api_assets/*.js'],
    languageOptions: {
      globals: {
        ...globals.browser,
      },
    },
  },
  // #endregion
  // #region lib
  {
    files: ['lib/**/*.js'],
    languageOptions: {
      globals: {
        process: 'readonly',
        internalBinding: 'readonly',
        primordials: 'readonly',
      },
    },
    rules: {
      'prefer-object-spread': 'error',
      'no-buffer-constructor': 'error',
      'no-restricted-syntax': [
        'error',
        ...restrictedSyntaxCommon,
        {
          selector: "CallExpression[callee.object.name='assert']:not([callee.property.name='ok']):not([callee.property.name='fail']):not([callee.property.name='ifError'])",
          message: 'Please only use simple assertions in ./lib',
        },
        {
          selector: 'NewExpression[callee.name=/Error$/]:not([callee.name=/^(AssertionError|NghttpError|AbortError|NodeAggregateError)$/])',
          message: 'Use an error exported by the internal/errors module.',
        },
        {
          selector: "CallExpression[callee.object.name='Error'][callee.property.name='captureStackTrace']",
          message: "Please use `require('internal/errors').hideStackFrames()` instead.",
        },
        {
          selector: "AssignmentExpression:matches([left.object.name='Error']):matches([left.name='prepareStackTrace'], [left.property.name='prepareStackTrace'])",
          message: "Use 'overrideStackTrace' from 'lib/internal/errors.js' instead of 'Error.prepareStackTrace'.",
        },
        {
          selector: "ThrowStatement > NewExpression[callee.name=/^ERR_[A-Z_]+$/] > ObjectExpression:first-child:not(:has([key.name='message']):has([key.name='code']):has([key.name='syscall']))",
          message: 'The context passed into SystemError constructor must have .code, .syscall and .message.',
        },
      ],
      'no-restricted-globals': [
        'error',
        {
          name: 'AbortController',
          message: "Use `const { AbortController } = require('internal/abort_controller');` instead of the global.",
        },
        {
          name: 'AbortSignal',
          message: "Use `const { AbortSignal } = require('internal/abort_controller');` instead of the global.",
        },
        {
          name: 'Blob',
          message: "Use `const { Blob } = require('buffer');` instead of the global.",
        },
        {
          name: 'BroadcastChannel',
          message: "Use `const { BroadcastChannel } = require('internal/worker/io');` instead of the global.",
        },
        {
          name: 'Buffer',
          message: "Use `const { Buffer } = require('buffer');` instead of the global.",
        },
        {
          name: 'ByteLengthQueuingStrategy',
          message: "Use `const { ByteLengthQueuingStrategy } = require('internal/webstreams/queuingstrategies')` instead of the global.",
        },
        {
          name: 'CompressionStream',
          message: "Use `const { CompressionStream } = require('internal/webstreams/compression')` instead of the global.",
        },
        {
          name: 'CountQueuingStrategy',
          message: "Use `const { CountQueuingStrategy } = require('internal/webstreams/queuingstrategies')` instead of the global.",
        },
        {
          name: 'CustomEvent',
          message: "Use `const { CustomEvent } = require('internal/event_target');` instead of the global.",
        },
        {
          name: 'DecompressionStream',
          message: "Use `const { DecompressionStream } = require('internal/webstreams/compression')` instead of the global.",
        },
        {
          name: 'DOMException',
          message: "Use lazy function `const { lazyDOMExceptionClass } = require('internal/util');` instead of the global.",
        },
        {
          name: 'Event',
          message: "Use `const { Event } = require('internal/event_target');` instead of the global.",
        },
        {
          name: 'EventTarget',
          message: "Use `const { EventTarget } = require('internal/event_target');` instead of the global.",
        },
        {
          name: 'File',
          message: "Use `const { File } = require('buffer');` instead of the global.",
        },
        {
          name: 'FormData',
          message: "Use `const { FormData } = require('internal/deps/undici/undici');` instead of the global.",
        },
        {
          name: 'Headers',
          message: "Use `const { Headers } = require('internal/deps/undici/undici');` instead of the global.",
        },
        // Intl is not available in primordials because it can be
        // disabled with --without-intl build flag.
        {
          name: 'Intl',
          message: 'Use `const { Intl } = globalThis;` instead of the global.',
        },
        {
          name: 'Iterator',
          message: 'Use `const { Iterator } = globalThis;` instead of the global.',
        },
        {
          name: 'MessageChannel',
          message: "Use `const { MessageChannel } = require('internal/worker/io');` instead of the global.",
        },
        {
          name: 'MessageEvent',
          message: "Use `const { MessageEvent } = require('internal/deps/undici/undici');` instead of the global.",
        },
        {
          name: 'MessagePort',
          message: "Use `const { MessagePort } = require('internal/worker/io');` instead of the global.",
        },
        {
          name: 'Navigator',
          message: "Use `const { Navigator } = require('internal/navigator');` instead of the global.",
        },
        {
          name: 'navigator',
          message: "Use `const { navigator } = require('internal/navigator');` instead of the global.",
        },
        {
          name: 'PerformanceEntry',
          message: "Use `const { PerformanceEntry } = require('perf_hooks');` instead of the global.",
        },
        {
          name: 'PerformanceMark',
          message: "Use `const { PerformanceMark } = require('perf_hooks');` instead of the global.",
        },
        {
          name: 'PerformanceMeasure',
          message: "Use `const { PerformanceMeasure } = require('perf_hooks');` instead of the global.",
        },
        {
          name: 'PerformanceObserverEntryList',
          message: "Use `const { PerformanceObserverEntryList } = require('perf_hooks');` instead of the global.",
        },
        {
          name: 'PerformanceObserver',
          message: "Use `const { PerformanceObserver } = require('perf_hooks');` instead of the global.",
        },
        {
          name: 'PerformanceResourceTiming',
          message: "Use `const { PerformanceResourceTiming } = require('perf_hooks');` instead of the global.",
        },
        {
          name: 'ReadableStream',
          message: "Use `const { ReadableStream } = require('internal/webstreams/readablestream')` instead of the global.",
        },
        {
          name: 'ReadableStreamDefaultReader',
          message: "Use `const { ReadableStreamDefaultReader } = require('internal/webstreams/readablestream')` instead of the global.",
        },
        {
          name: 'ReadableStreamBYOBReader',
          message: "Use `const { ReadableStreamBYOBReader } = require('internal/webstreams/readablestream')` instead of the global.",
        },
        {
          name: 'ReadableStreamBYOBRequest',
          message: "Use `const { ReadableStreamBYOBRequest } = require('internal/webstreams/readablestream')` instead of the global.",
        },
        {
          name: 'ReadableByteStreamController',
          message: "Use `const { ReadableByteStreamController } = require('internal/webstreams/readablestream')` instead of the global.",
        },
        {
          name: 'ReadableStreamDefaultController',
          message: "Use `const { ReadableStreamDefaultController } = require('internal/webstreams/readablestream')` instead of the global.",
        },
        {
          name: 'Request',
          message: "Use `const { Request } = require('internal/deps/undici/undici');` instead of the global.",
        },
        {
          name: 'Response',
          message: "Use `const { Response } = require('internal/deps/undici/undici');` instead of the global.",
        },
        // ShadowRealm is not available in primordials because it can be
        // disabled with --no-harmony-shadow-realm CLI flag.
        {
          name: 'ShadowRealm',
          message: 'Use `const { ShadowRealm } = globalThis;` instead of the global.',
        },
        // SharedArrayBuffer is not available in primordials because it can be
        // disabled with --no-harmony-sharedarraybuffer CLI flag.
        {
          name: 'SharedArrayBuffer',
          message: 'Use `const { SharedArrayBuffer } = globalThis;` instead of the global.',
        },
        {
          name: 'TextDecoder',
          message: "Use `const { TextDecoder } = require('internal/encoding');` instead of the global.",
        },
        {
          name: 'TextDecoderStream',
          message: "Use `const { TextDecoderStream } = require('internal/webstreams/encoding')` instead of the global.",
        },
        {
          name: 'TextEncoder',
          message: "Use `const { TextEncoder } = require('internal/encoding');` instead of the global.",
        },
        {
          name: 'TextEncoderStream',
          message: "Use `const { TextEncoderStream } = require('internal/webstreams/encoding')` instead of the global.",
        },
        {
          name: 'TransformStream',
          message: "Use `const { TransformStream } = require('internal/webstreams/transformstream')` instead of the global.",
        },
        {
          name: 'TransformStreamDefaultController',
          message: "Use `const { TransformStreamDefaultController } = require('internal/webstreams/transformstream')` instead of the global.",
        },
        {
          name: 'URL',
          message: "Use `const { URL } = require('internal/url');` instead of the global.",
        },
        {
          name: 'URLSearchParams',
          message: "Use `const { URLSearchParams } = require('internal/url');` instead of the global.",
        },
        // WebAssembly is not available in primordials because it can be
        // disabled with --jitless CLI flag.
        {
          name: 'WebAssembly',
          message: 'Use `const { WebAssembly } = globalThis;` instead of the global.',
        },
        {
          name: 'WritableStream',
          message: "Use `const { WritableStream } = require('internal/webstreams/writablestream')` instead of the global.",
        },
        {
          name: 'WritableStreamDefaultWriter',
          message: "Use `const { WritableStreamDefaultWriter } = require('internal/webstreams/writablestream')` instead of the global.",
        },
        {
          name: 'WritableStreamDefaultController',
          message: "Use `const { WritableStreamDefaultController } = require('internal/webstreams/writablestream')` instead of the global.",
        },
        {
          name: 'atob',
          message: "Use `const { atob } = require('buffer');` instead of the global.",
        },
        {
          name: 'btoa',
          message: "Use `const { btoa } = require('buffer');` instead of the global.",
        },
        {
          name: 'clearImmediate',
          message: "Use `const { clearImmediate } = require('timers');` instead of the global.",
        },
        {
          name: 'clearInterval',
          message: "Use `const { clearInterval } = require('timers');` instead of the global.",
        },
        {
          name: 'clearTimeout',
          message: "Use `const { clearTimeout } = require('timers');` instead of the global.",
        },
        {
          name: 'console',
          message: "Use `const console = require('internal/console/global');` instead of the global.",
        },
        {
          name: 'crypto',
          message: "Use `const { crypto } = require('internal/crypto/webcrypto');` instead of the global.",
        },
        {
          name: 'Crypto',
          message: "Use `const { Crypto } = require('internal/crypto/webcrypto');` instead of the global.",
        },
        {
          name: 'CryptoKey',
          message: "Use `const { CryptoKey } = require('internal/crypto/webcrypto');` instead of the global.",
        },
        {
          name: 'fetch',
          message: "Use `const { fetch } = require('internal/deps/undici/undici');` instead of the global.",
        },
        {
          name: 'global',
          message: 'Use `const { globalThis } = primordials;` instead of `global`.',
        },
        {
          name: 'globalThis',
          message: 'Use `const { globalThis } = primordials;` instead of the global.',
        },
        {
          name: 'performance',
          message: "Use `const { performance } = require('perf_hooks');` instead of the global.",
        },
        {
          name: 'queueMicrotask',
          message: "Use `const { queueMicrotask } = require('internal/process/task_queues');` instead of the global.",
        },
        {
          name: 'setImmediate',
          message: "Use `const { setImmediate } = require('timers');` instead of the global.",
        },
        {
          name: 'setInterval',
          message: "Use `const { setInterval } = require('timers');` instead of the global.",
        },
        {
          name: 'setTimeout',
          message: "Use `const { setTimeout } = require('timers');` instead of the global.",
        },
        {
          name: 'structuredClone',
          message: "Use `const { structuredClone } = internalBinding('messaging');` instead of the global.",
        },
        {
          name: 'SubtleCrypto',
          message: "Use `const { SubtleCrypto } = require('internal/crypto/webcrypto');` instead of the global.",
        },
      ],
      'no-restricted-modules': [
        'error',
        {
          name: 'url',
          message: 'Require `internal/url` instead of `url`.',
        },
      ],
      '@stylistic/js/no-mixed-operators': [
        'error',
        {
          groups: [['&&', '||']],
        },
      ],
      // Custom rules in tools/eslint-rules
      'node-core/alphabetize-errors': 'error',
      'node-core/alphabetize-primordials': 'error',
      'node-core/avoid-prototype-pollution': 'error',
      'node-core/lowercase-name-for-primitive': 'error',
      'node-core/non-ascii-character': 'error',
      'node-core/no-array-destructuring': 'error',
      'node-core/prefer-primordials': [
        'error',
        { name: 'AggregateError' },
        { name: 'Array' },
        { name: 'ArrayBuffer' },
        { name: 'Atomics' },
        { name: 'BigInt' },
        { name: 'BigInt64Array' },
        { name: 'BigUint64Array' },
        { name: 'Boolean' },
        { name: 'DataView' },
        { name: 'Date' },
        { name: 'decodeURI' },
        { name: 'decodeURIComponent' },
        { name: 'encodeURI' },
        { name: 'encodeURIComponent' },
        { name: 'escape' },
        { name: 'eval' },
        {
          name: 'Error',
          ignore: [
            'prepareStackTrace',
            'stackTraceLimit',
          ],
        },
        { name: 'EvalError' },
        {
          name: 'FinalizationRegistry',
          into: 'Safe',
        },
        { name: 'Float32Array' },
        { name: 'Float64Array' },
        { name: 'Function' },
        { name: 'Int16Array' },
        { name: 'Int32Array' },
        { name: 'Int8Array' },
        {
          name: 'isFinite',
          into: 'Number',
        },
        {
          name: 'isNaN',
          into: 'Number',
        },
        { name: 'JSON' },
        {
          name: 'Map',
          into: 'Safe',
        },
        { name: 'Math' },
        { name: 'Number' },
        { name: 'Object' },
        {
          name: 'parseFloat',
          into: 'Number',
        },
        {
          name: 'parseInt',
          into: 'Number',
        },
        { name: 'Proxy' },
        { name: 'Promise' },
        { name: 'RangeError' },
        { name: 'ReferenceError' },
        { name: 'Reflect' },
        { name: 'RegExp' },
        {
          name: 'Set',
          into: 'Safe',
        },
        { name: 'String' },
        { name: 'Symbol' },
        { name: 'SyntaxError' },
        { name: 'TypeError' },
        { name: 'Uint16Array' },
        { name: 'Uint32Array' },
        { name: 'Uint8Array' },
        { name: 'Uint8ClampedArray' },
        { name: 'unescape' },
        { name: 'URIError' },
        {
          name: 'WeakMap',
          into: 'Safe',
        },
        {
          name: 'WeakRef',
          into: 'Safe',
        },
        {
          name: 'WeakSet',
          into: 'Safe',
        },
      ],
    },
  },
  {
    files: ['lib/internal/modules/**/*.js'],
    rules: {
      'curly': 'error',
    },
  },
  {
    files: ['lib/internal/per_context/primordials.js'],
    rules: {
      'node-core/alphabetize-primordials': [
        'error',
        { enforceTopPosition: false },
      ],
    },
  },
  {
    files: ['lib/internal/test_runner/**/*.js'],
    rules: {
      'node-core/set-proto-to-null-in-object': 'error',
    },
  },
  // #endregion
  // #region test
  {
    files: ['test/**/*.{js,mjs,cjs}'],
    languageOptions: {
      globals: {
        ...globals.node,
      },
    },
    rules: {
      'multiline-comment-style': [
        'error',
        'separate-lines',
      ],
      'prefer-const': 'error',
      'symbol-description': 'off',
      'no-restricted-syntax': [
        // TODO: copy
        'error',
        ...restrictedSyntaxCommon,
        {
          selector: "CallExpression:matches([callee.name='deepStrictEqual'], [callee.property.name='deepStrictEqual']):matches([arguments.1.type='Literal']:not([arguments.1.regex]), [arguments.1.type='Identifier'][arguments.1.name='undefined'])",
          message: 'Use strictEqual instead of deepStrictEqual for literals or undefined.',
        },
        {
          selector: "CallExpression:matches([callee.name='notDeepStrictEqual'], [callee.property.name='notDeepStrictEqual']):matches([arguments.1.type='Literal']:not([arguments.1.regex]), [arguments.1.type='Identifier'][arguments.1.name='undefined'])",
          message: 'Use notStrictEqual instead of notDeepStrictEqual for literals or undefined.',
        },
        {
          selector: "CallExpression:matches([callee.name='deepStrictEqual'], [callee.property.name='deepStrictEqual'])[arguments.2.type='Literal']",
          message: 'Do not use a literal for the third argument of assert.deepStrictEqual()',
        },
        {
          selector: "CallExpression:matches([callee.name='doesNotThrow'], [callee.property.name='doesNotThrow'])",
          message: 'Do not use `assert.doesNotThrow()`. Write the code without the wrapper and add a comment instead.',
        },
        {
          selector: "CallExpression:matches([callee.name='doesNotReject'], [callee.property.name='doesNotReject'])",
          message: 'Do not use `assert.doesNotReject()`. Write the code without the wrapper and add a comment instead.',
        },
        {
          selector: "CallExpression:matches([callee.name='rejects'], [callee.property.name='rejects'])[arguments.length<2]",
          message: '`assert.rejects()` must be invoked with at least two arguments.',
        },
        {
          selector: "CallExpression[callee.property.name='strictEqual'][arguments.2.type='Literal']",
          message: 'Do not use a literal for the third argument of assert.strictEqual()',
        },
        {
          selector: "CallExpression:matches([callee.name='throws'], [callee.property.name='throws'])[arguments.1.type='Literal']:not([arguments.1.regex])",
          message: 'Use an object as second argument of `assert.throws()`.',
        },
        {
          selector: "CallExpression:matches([callee.name='throws'], [callee.property.name='throws'])[arguments.length<2]",
          message: '`assert.throws()` must be invoked with at least two arguments.',
        },
        {
          selector: "CallExpression:matches([callee.name='notDeepStrictEqual'], [callee.property.name='notDeepStrictEqual'])[arguments.0.type='Literal']:not([arguments.1.type='Literal']):not([arguments.1.type='ObjectExpression']):not([arguments.1.type='ArrayExpression']):not([arguments.1.type='UnaryExpression'])",
          message: 'The first argument should be the `actual`, not the `expected` value.',
        },
        {
          selector: "CallExpression:matches([callee.name='notStrictEqual'], [callee.property.name='notStrictEqual'])[arguments.0.type='Literal']:not([arguments.1.type='Literal']):not([arguments.1.type='ObjectExpression']):not([arguments.1.type='ArrayExpression']):not([arguments.1.type='UnaryExpression'])",
          message: 'The first argument should be the `actual`, not the `expected` value.',
        },
        {
          selector: "CallExpression:matches([callee.name='deepStrictEqual'], [callee.property.name='deepStrictEqual'])[arguments.0.type='Literal']:not([arguments.1.type='Literal']):not([arguments.1.type='ObjectExpression']):not([arguments.1.type='ArrayExpression']):not([arguments.1.type='UnaryExpression'])",
          message: 'The first argument should be the `actual`, not the `expected` value.',
        },
        {
          selector: "CallExpression:matches([callee.name='strictEqual'], [callee.property.name='strictEqual'])[arguments.0.type='Literal']:not([arguments.1.type='Literal']):not([arguments.1.type='ObjectExpression']):not([arguments.1.type='ArrayExpression']):not([arguments.1.type='UnaryExpression'])",
          message: 'The first argument should be the `actual`, not the `expected` value.',
        },
        {
          selector: "VariableDeclarator > CallExpression:matches([callee.name='debuglog'], [callee.property.name='debuglog']):not([arguments.0.value='test'])",
          message: "Use 'test' as debuglog value in tests.",
        },
        {
          selector: 'CallExpression:matches([callee.object.name="common"][callee.property.name=/^must(Not)?Call/],[callee.name="mustCall"],[callee.name="mustCallAtLeast"],[callee.name="mustNotCall"])>:first-child[type=/FunctionExpression$/][body.body.length=0]',
          message: 'Do not use an empty function, omit the parameter altogether.',
        },
        {
          selector: "ExpressionStatement>CallExpression:matches([callee.name='rejects'], [callee.object.name='assert'][callee.property.name='rejects'])",
          message: 'Calling `assert.rejects` without `await` or `.then(common.mustCall())` will not detect never-settling promises.',
        },
        {
          selector: "Identifier[name='webcrypto']",
          message: 'Use `globalThis.crypto`.',
        },
      ],
      '@stylistic/js/comma-dangle': [
        'error',
        'always-multiline',
      ],
      'node-core/prefer-assert-iferror': 'error',
      'node-core/prefer-assert-methods': 'error',
      'node-core/prefer-common-mustnotcall': 'error',
      'node-core/prefer-common-mustsucceed': 'error',
      'node-core/crypto-check': 'error',
      'node-core/eslint-check': 'error',
      'node-core/async-iife-no-unused-result': 'error',
      'node-core/inspector-check': 'error',
      // `common` module is mandatory in tests.
      'node-core/required-modules': [
        'error',
        { common: 'common(/index\\.(m)?js)?$' },
      ],
      'node-core/require-common-first': 'error',
      'node-core/no-duplicate-requires': 'off',
    },
  },
  {
    files: ['test/common/**/*.{js,mjs,cjs}'],
    rules: {
      'node-core/required-modules': 'off',
      'node-core/require-common-first': 'off',
    },
  },
  {
    files: [
      'test/es-module/**/*.{js,mjs}',
      'test/parallel/**/*.{js,mjs}',
      'test/sequential/**/*.{js,mjs}',
    ],
    rules: {
      '@stylistic/js/comma-dangle': [
        'error',
        {
          arrays: 'always-multiline',
          exports: 'always-multiline',
          functions: 'only-multiline',
          imports: 'always-multiline',
          objects: 'only-multiline',
        },
      ],
    },
  },
  {
    files: [
      'test/es-module/test-esm-example-loader.js',
      'test/es-module/test-esm-type-flag.js',
      'test/es-module/test-esm-type-flag-alias.js',
      'test/es-module/test-require-module-detect-entry-point.js',
      'test/es-module/test-require-module-detect-entry-point-aou.js',
    ],
    languageOptions: {
      parserOptions: { sourceType: 'module' },
    },
  },
  {
    files: [
      'test/wpt/**/*.{js,mjs,cjs}',
    ],
    rules: {
      'node-core/required-modules': 'off',
      'node-core/require-common-first': 'off',
    },
  },
  // #endregion
  // #region tools
  {
    files: ['tools/**/*.{js,mjs,cjs}'],
    languageOptions: {
      globals: {
        ...globals.node,
      },
    },
    rules: {
      'camelcase': [
        'error',
        {
          properties: 'never',
          ignoreDestructuring: true,
          allow: ['child_process'],
        },
      ],
      'no-unused-vars': [
        'error',
        { args: 'after-used' },
      ],
      'prefer-arrow-callback': 'error',
    },
  },
  // #endregion
];
