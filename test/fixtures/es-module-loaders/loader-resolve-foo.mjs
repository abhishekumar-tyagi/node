import { writeSync } from 'node:fs';

export async function resolve(specifier, context, next) {
  // This check is needed to make sure that we don't prevent the
  // resolution from follow-up loaders, or `'node:fs'` that we
  // use instead of `console.log`. It wouldn't be a problem
  // in real life because loaders aren't supposed to break the
  // resolution, but the ones used in our tests do, for convenience.
  if (specifier.includes('loader') || specifier === 'node:fs') {
    return next(specifier, context);
  }

  writeSync(1, 'resolve foo\n'); // This log is deliberate
  return next('file:///foo.mjs');
}
