'use strict';

const {
  ArrayPrototypePush,
  MathMax
} = primordials;
const colors = require('internal/util/colors');
const { symbols } = require('internal/test_runner/reporter/utils');

module.exports = async function* dot(source) {
  let count = 0;
  let columns = getLineLength();
  const failedTests = [];

  for await (const { type, data } of source) {
    if (type === 'test:pass') {
      yield '.';
    }
    if (type === 'test:fail') {
      yield 'X';
      ArrayPrototypePush(failedTests, data);
    }
    if ((type === 'test:fail' || type === 'test:pass') && ++count === columns) {
      yield '\n';

      // Getting again in case the terminal was resized.
      columns = getLineLength();
      count = 0;
    }
  }
  yield '\n';

  if (failedTests.length > 0) {
    yield `${colors.red}${symbols['test:fail']} Failed tests:${colors.white}\n`;
    for (const test of failedTests) {
      const message = `${colors.red}${symbols['test:fail']} ${test.name} ${colors.gray}(${test.details.duration_ms}ms)${colors.white}\n`;
      yield message;
    }
  }
};

function getLineLength() {
  return MathMax(process.stdout.columns ?? 20, 20);
}
