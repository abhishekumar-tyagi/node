import '../common/index.mjs';
import { path } from '../common/fixtures.mjs';
import { match, notStrictEqual } from 'node:assert';
import { execPath } from 'node:process';
import { describe, it } from 'node:test';

import spawn from './helper.spawnAsPromised.mjs';


describe('ESM: importing a module with syntax error(s)', { concurrency: true }, () => {
  it('should throw', async () => {
    const { code, stderr } = await spawn(execPath, [
      path('es-module-loaders', 'syntax-error.mjs'),
    ]);
    match(stderr, /SyntaxError:/);
    notStrictEqual(code, 0);
  });
});
