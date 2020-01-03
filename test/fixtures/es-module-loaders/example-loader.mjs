import url from 'url';
import path from 'path';
import process from 'process';
import { builtinModules } from 'module';

const JS_EXTENSIONS = new Set(['.js', '.mjs']);

const baseURL = new url.URL('file://');
baseURL.pathname = process.cwd() + '/';

export function resolve({specifier, parentURL = baseURL, defaultResolve}) {
  if (builtinModules.includes(specifier)) {
    return {
      url: specifier,
      format: 'builtin'
    };
  }
  if (/^\.{1,2}[/]/.test(specifier) !== true && !specifier.startsWith('file:')) {
    // For node_modules support:
    // return defaultResolve({specifier, parentURL, defaultResolve});
    throw new Error(
      `imports must be URLs or begin with './', or '../'; '${specifier}' does not`);
  }
  const resolved = new url.URL(specifier, parentURL);
  const ext = path.extname(resolved.pathname);
  if (!JS_EXTENSIONS.has(ext)) {
    throw new Error(
      `Cannot load file with non-JavaScript file extension ${ext}.`);
  }
  return {
    url: resolved.href,
    format: 'module'
  };
}
