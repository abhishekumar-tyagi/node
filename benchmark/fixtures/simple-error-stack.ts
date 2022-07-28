'use strict';

// Compile with `tsc --inlineSourceMap benchmark/fixtures/simple-error-stack.ts`.

const lorem = 'Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.';

function simpleErrorStack() {
  try {
    (lorem as any).BANG();
  } catch (e) {
    return e.stack;
  }
}

export {
  simpleErrorStack,
};
