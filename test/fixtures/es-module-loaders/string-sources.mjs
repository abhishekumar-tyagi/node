const SOURCES = {
  __proto__: null,
  'test:Array': ['1', '2'], // both `1,2` and `12` are valid ESM
  'test:ArrayBuffer': new ArrayBuffer(0),
  'test:null': null,
  'test:Object': {},
  'test:SharedArrayBuffer': new SharedArrayBuffer(0),
  'test:string': '',
  'test:String': new String(''),
  'test:Uint8Array': new Uint8Array(0),
  'test:undefined': undefined,
}
export function resolve(specifier) {
  if (specifier.startsWith('test:')) {
    return { url: specifier };
  }
  return null;
}
export function getFormat(href) {
  if (href.startsWith('test:')) {
    return { format: 'module' };
  }
  return null;
}
export function getSource(href) {
  if (href.startsWith('test:')) {
    return { source: SOURCES[href] };
  }
  return null;
}
