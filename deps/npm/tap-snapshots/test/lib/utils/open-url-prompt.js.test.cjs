/* IMPORTANT
 * This snapshot file is auto-generated, but designed for humans.
 * It should be checked into source control and tracked carefully.
 * Re-generate by setting TAP_SNAPSHOT=1 and running tests.
 * Make sure to inspect the output below.  Do not ignore changes!
 */
'use strict'
exports[`test/lib/utils/open-url-prompt.js TAP does not error when opener can not find command > Outputs extra Browser unavailable message and url 1`] = `
npm home:
https://www.npmjs.com
Browser unavailable.  Please open the URL manually:
  https://www.npmjs.com

`

exports[`test/lib/utils/open-url-prompt.js TAP opens a url > must match snapshot 1`] = `
npm home:
https://www.npmjs.com
`

exports[`test/lib/utils/open-url-prompt.js TAP prints json output > must match snapshot 1`] = `
{"title":"npm home","url":"https://www.npmjs.com"}
`
