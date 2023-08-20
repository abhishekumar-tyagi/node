import { spawnPromisified } from '../common/index.mjs';
import { fileURL, path } from '../common/fixtures.mjs';
import { doesNotMatch, match, notStrictEqual } from 'node:assert';
import { execPath } from 'node:process';
import { describe, it } from 'node:test';


describe('ESM: loader with syntax error', { concurrency: true }, () => {
  it('should crash the node process', async () => {
    const { code, stderr } = await spawnPromisified(execPath, [
      '--no-warnings',
      '--experimental-loader',
      fileURL('es-module-loaders', 'syntax-error.mjs').href,
      path('print-error-message.js'),
    ]);

    console.log(stderr);

    match(stderr, /SyntaxError \[Error\]:/);
    doesNotMatch(stderr, /Bad command or file name/);
    notStrictEqual(code, 0);
  });
});
