#!/usr/bin/env node
// Usage: tools/update-author.js [--dry]
// Passing --dry will redirect output to stdout rather than write to 'AUTHORS'.
'use strict';
const { spawn } = require('child_process');
const fs = require('fs');
const readline = require('readline');

const log = spawn(
  'git',
  // Inspect author name/email and body.
  ['log', '--reverse', '--format=Author: %aN <%aE>\n%b'], {
    stdio: ['inherit', 'pipe', 'inherit']
  });
const rl = readline.createInterface({ input: log.stdout });

let output;
if (process.argv.includes('--dry'))
  output = process.stdout;
else
  output = fs.createWriteStream('AUTHORS');

output.write('# Authors ordered by first contribution.\n\n');

const seen = new Set();

// Support regular git author metadata, as well as `Author:` and
// `Co-authored-by:` in the message body. Both have been used in the past
// to indicate multiple authors per commit, with the latter standardized
// by GitHub now.
const authorRe = /(?:^Author:|^Co-authored-by:)\s+([^<]+)\s+(<[^>]+>)/i;
rl.on('line', (line) => {
  const match = line.match(authorRe);
  if (!match) return;

  const [ _, author, email ] = match;  // eslint-disable-line no-unused-vars
  if (seen.has(email) ||
      /@chromium\.org/.test(email) ||
      email === '<erik.corry@gmail.com>') {
    return;
  }

  seen.add(email);
  output.write(`${author} ${email}\n`);
});

rl.on('close', () => {
  output.end('\n# Generated by tools/update-authors.js\n');
});
