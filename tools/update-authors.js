#!/usr/bin/env node
// Usage: tools/update-author.js [--dry]
// Passing --dry will redirect output to stdout rather than write to 'AUTHORS'.
'use strict';
const { spawn } = require('child_process');
const path = require('path');
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

const mailmap = new Map();
{
  const lines = fs.readFileSync(path.resolve(__dirname, '../', '.mailmap'),
                                { encoding: 'utf8' }).split('\n');
  for (let line of lines) {
    line = line.trim();
    if (line.startsWith('#') || line === '') continue;

    let match;
    // Replaced Name <original@example.com>
    if (match = line.match(/^([^<]+)\s+(<[^>]+>)$/)) {
      mailmap.set(match[2], { author: match[1] });
    // <replaced@example.com> <original@example.com>
    } else if (match = line.match(/^<([^>]+)>\s+(<[^>]+>)$/)) {
      mailmap.set(match[2], { email: match[1] });
    // Replaced Name <replaced@example.com> <original@example.com>
    } else if (match = line.match(/^([^<]+)\s+(<[^>]+>)\s+(<[^>]+>)$/)) {
      mailmap.set(match[3], {
        author: match[1], email: match[2]
      });
    // Replaced Name <replaced@example.com> Original Name <original@example.com>
    } else if (match =
        line.match(/^([^<]+)\s+(<[^>]+>)\s+([^<]+)\s+(<[^>]+>)$/)) {
      mailmap.set(match[3] + '\0' + match[4], {
        author: match[1], email: match[2]
      });
    } else {
      console.warn('Unknown .mailmap format:', line);
    }
  }
}

const seen = new Set();

// Support regular git author metadata, as well as `Author:` and
// `Co-authored-by:` in the message body. Both have been used in the past
// to indicate multiple authors per commit, with the latter standardized
// by GitHub now.
const authorRe =
  /(^Author:|^Co-authored-by:)\s+(?<author>[^<]+)\s+(?<email><[^>]+>)/i;
rl.on('line', (line) => {
  const match = line.match(authorRe);
  if (!match) return;

  let { author, email } = match.groups;

  const replacement = mailmap.get(author + '\0' + email) || mailmap.get(email);
  if (replacement) {
    ({ author, email } = { author, email, ...replacement });
  }

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
