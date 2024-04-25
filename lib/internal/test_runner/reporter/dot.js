'use strict';
const {
  ArrayPrototypePush,
  MathMax,
} = primordials;
const colors = require('internal/util/colors');
const { reporterUnicodeSymbolMap, formatTestReport, indent } = require('internal/test_runner/reporter/utils');

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
    yield `${colors.red}${reporterUnicodeSymbolMap['test:fail']} Failed tests:${colors.white}\n`;
      for (const test of failedTests) {
        const indentation = indent(test.nesting);
        const formattedReport = formatTestReport('test:fail', test, '', indentation, false);
        yield formattedReport;
      }
  }
};

function getLineLength() {
  return MathMax(process.stdout.columns ?? 20, 20);
}
