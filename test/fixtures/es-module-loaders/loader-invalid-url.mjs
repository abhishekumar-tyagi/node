/* eslint-disable node-core/required-modules */
export async function resolve({ specifier, parentURL }, defaultResolve, loader) {
  if (parentURL && specifier === '../fixtures/es-modules/test-esm-ok.mjs') {
    return {
      url: specifier,
      format: 'esm'
    };
  }
  return defaultResolve({specifier, parentURL}, defaultResolve, loader);
}
