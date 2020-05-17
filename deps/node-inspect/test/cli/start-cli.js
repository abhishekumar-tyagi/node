'use strict';
const spawn = require('child_process').spawn;

// This allows us to keep the helper inside of `test/` without tap warning
// about "pending" test files.
const tap = require('tap');
tap.test('startCLI', (t) => t.end());

const CLI =
  process.env.USE_EMBEDDED_NODE_INSPECT === '1' ?
    'inspect' :
    require.resolve('../../cli.js');

const BREAK_MESSAGE = new RegExp('(?:' + [
  'assert', 'break', 'break on start', 'debugCommand',
  'exception', 'other', 'promiseRejection',
].join('|') + ') in', 'i');

function isPreBreak(output) {
  return /Break on start/.test(output) && /1 \(function \(exports/.test(output);
}

function startCLI(args, flags = [], spawnOpts = {}) {
  const child = spawn(process.execPath, [...flags, CLI, ...args], spawnOpts);
  let isFirstStdoutChunk = true;

  const outputBuffer = [];
  function bufferOutput(chunk) {
    if (isFirstStdoutChunk) {
      isFirstStdoutChunk = false;
      outputBuffer.push(chunk.replace(/^debug>\s*/, ''));
    } else {
      outputBuffer.push(chunk);
    }
  }

  function getOutput() {
    return outputBuffer.join('').toString()
      .replace(/^[^\n]*?[\b]/mg, '');
  }

  child.stdout.setEncoding('utf8');
  child.stdout.on('data', bufferOutput);
  child.stderr.setEncoding('utf8');
  child.stderr.on('data', bufferOutput);

  if (process.env.VERBOSE === '1') {
    child.stdout.pipe(process.stderr);
    child.stderr.pipe(process.stderr);
  }

  return {
    flushOutput() {
      const output = this.output;
      outputBuffer.length = 0;
      return output;
    },

    waitFor(pattern, timeout = 2000) {
      function checkPattern(str) {
        if (Array.isArray(pattern)) {
          return pattern.every((p) => p.test(str));
        }
        return pattern.test(str);
      }

      return new Promise((resolve, reject) => {
        function checkOutput() {
          if (checkPattern(getOutput())) {
            tearDown(); // eslint-disable-line no-use-before-define
            resolve();
          }
        }

        function onChildExit() {
          tearDown(); // eslint-disable-line no-use-before-define
          reject(new Error(
            `Child quit while waiting for ${pattern}; found: ${this.output}`));
        }

        const timer = setTimeout(() => {
          tearDown(); // eslint-disable-line no-use-before-define
          reject(new Error([
            `Timeout (${timeout}) while waiting for ${pattern}`,
            `found: ${this.output}`,
          ].join('; ')));
        }, timeout);

        function tearDown() {
          clearTimeout(timer);
          child.stdout.removeListener('data', checkOutput);
          child.removeListener('exit', onChildExit);
        }

        child.on('exit', onChildExit);
        child.stdout.on('data', checkOutput);
        checkOutput();
      });
    },

    waitForPrompt(timeout = 2000) {
      return this.waitFor(/>\s+$/, timeout);
    },

    waitForInitialBreak(timeout = 2000) {
      return this.waitFor(/break (?:on start )?in/i, timeout)
        .then(() => {
          if (isPreBreak(this.output)) {
            return this.command('next', false)
              .then(() => this.waitFor(/break in/, timeout));
          }
        });
    },

    get breakInfo() {
      const output = this.output;
      const breakMatch =
        output.match(/break (?:on start )?in ([^\n]+):(\d+)\n/i);

      if (breakMatch === null) {
        throw new Error(
          `Could not find breakpoint info in ${JSON.stringify(output)}`);
      }
      return { filename: breakMatch[1], line: +breakMatch[2] };
    },

    ctrlC() {
      return this.command('.interrupt');
    },

    get output() {
      return getOutput();
    },

    get rawOutput() {
      return outputBuffer.join('').toString();
    },

    parseSourceLines() {
      return getOutput().split('\n')
        .map((line) => line.match(/(?:\*|>)?\s*(\d+)/))
        .filter((match) => match !== null)
        .map((match) => +match[1]);
    },

    writeLine(input, flush = true) {
      if (flush) {
        this.flushOutput();
      }
      if (process.env.VERBOSE === '1') {
        process.stderr.write(`< ${input}\n`);
      }
      child.stdin.write(input);
      child.stdin.write('\n');
    },

    command(input, flush = true) {
      this.writeLine(input, flush);
      return this.waitForPrompt();
    },

    stepCommand(input) {
      this.writeLine(input, true);
      return this
        .waitFor(BREAK_MESSAGE)
        .then(() => this.waitForPrompt());
    },

    quit() {
      return new Promise((resolve) => {
        child.stdin.end();
        child.on('exit', resolve);
      });
    },
  };
}
module.exports = startCLI;
