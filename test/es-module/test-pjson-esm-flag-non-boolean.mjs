// Flags: --experimental-modules
/* eslint-disable required-modules */
import m from '../fixtures/es-modules/esm-non-boolean/module';
import assert from 'assert';

assert.strictEqual(m, 'cjs');
