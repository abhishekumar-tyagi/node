'use strict';

const common = require('../common');

// This tests the CLI parser for our benchmark suite.

const assert = require('assert');

const CLI = require('../../benchmark/_cli.js');

const originalArgv = process.argv;

function testFilterPattern(filters, excludes, filename, expectedResult) {
  process.argv = process.argv.concat(...filters.map((p) => ['--filter', p]));
  process.argv = process.argv.concat(...excludes.map((p) => ['--exclude', p]));
  process.argv = process.argv.concat(['bench']);

  const cli = new CLI('', { 'arrayArgs': ['filter', 'exclude'] });
  assert.deepStrictEqual(cli.shouldSkip(filename), expectedResult);

  process.argv = originalArgv;
}


testFilterPattern([], [], 'foo', false);

testFilterPattern(['foo'], [], 'foo', false);
testFilterPattern(['foo'], [], 'bar', true);
testFilterPattern(['foo', 'bar'], [], 'foo', false);
testFilterPattern(['foo', 'bar'], [], 'bar', false);

testFilterPattern([], ['foo'], 'foo', true);
testFilterPattern([], ['foo'], 'bar', false);
testFilterPattern([], ['foo', 'bar'], 'foo', true);
testFilterPattern([], ['foo', 'bar'], 'bar', true);

testFilterPattern(['foo'], ['bar'], 'foo', false);
testFilterPattern(['foo'], ['bar'], 'foo-bar', true);

function testNoSettingsPattern(filters, excludes, filename, expectedResult) {
  process.argv = process.argv.concat(...filters.map((p) => ['--filter', p]));
  process.argv = process.argv.concat(...excludes.map((p) => ['--exclude', p]));
  process.argv = process.argv.concat(['bench']);
  try {
    const cli = new CLI('');
    assert.deepStrictEqual(cli.shouldSkip(filename), expectedResult);
  } catch {
    common.mustNotCall('If settings param is null, shouldn\'t throw an error');
  }
  process.argv = originalArgv;
}

testNoSettingsPattern([], []);
testNoSettingsPattern([], [], 'foo', false);

testNoSettingsPattern(['foo'], [], 'foo', false);
testNoSettingsPattern(['foo'], [], 'bar', true);
testNoSettingsPattern(['foo', 'bar'], [], 'foo', false);
testNoSettingsPattern(['foo', 'bar'], [], 'bar', false);

testNoSettingsPattern([], ['foo'], 'foo', true);
testNoSettingsPattern([], ['foo'], 'bar', false);
testNoSettingsPattern([], ['foo', 'bar'], 'foo', true);
testNoSettingsPattern([], ['foo', 'bar'], 'bar', true);

testNoSettingsPattern(['foo'], ['bar'], 'foo', false);
testNoSettingsPattern(['foo'], ['bar'], 'foo-bar', true);
