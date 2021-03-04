// pass in an arborist object, and it'll output the data about what
// was done, what was audited, etc.
//
// added ## packages, removed ## packages, and audited ## packages in 19.157s
//
// 1 package is looking for funding
//   run `npm fund` for details
//
// found 37 vulnerabilities (5 low, 7 moderate, 25 high)
//   run `npm audit fix` to fix them, or `npm audit` for details

const log = require('npmlog')
const output = require('./output.js')
const { depth } = require('treeverse')
const ms = require('ms')
const auditReport = require('npm-audit-report')
const { readTree: getFundingInfo } = require('libnpmfund')
const auditError = require('./audit-error.js')

// TODO: output JSON if flatOptions.json is true
const reifyOutput = (npm, arb) => {
  // don't print any info in --silent mode
  if (log.levels[log.level] > log.levels.error)
    return

  const { diff, actualTree } = arb

  // note: fails and crashes if we're running audit fix and there was an error
  // which is a good thing, because there's no point printing all this other
  // stuff in that case!
  const auditReport = auditError(npm, arb.auditReport) ? null : arb.auditReport

  const summary = {
    added: 0,
    removed: 0,
    changed: 0,
    audited: auditReport && !auditReport.error ? actualTree.inventory.size : 0,
    funding: 0,
  }

  if (diff) {
    depth({
      tree: diff,
      visit: d => {
        switch (d.action) {
          case 'REMOVE':
            summary.removed++
            break
          case 'ADD':
            actualTree.inventory.has(d.ideal) && summary.added++
            break
          case 'CHANGE':
            summary.changed++
            break
          default:
            return
        }
        const node = d.actual || d.ideal
        log.silly(d.action, node.location)
      },
      getChildren: d => d.children,
    })
  }

  if (npm.flatOptions.fund) {
    const fundingInfo = getFundingInfo(actualTree, { countOnly: true })
    summary.funding = fundingInfo.length
  }

  if (npm.flatOptions.json) {
    if (auditReport) {
      summary.audit = npm.command === 'audit' ? auditReport
        : auditReport.toJSON().metadata
    }
    output(JSON.stringify(summary, 0, 2))
  } else {
    packagesChangedMessage(npm, summary)
    packagesFundingMessage(summary)
    printAuditReport(npm, auditReport)
  }
}

// if we're running `npm audit fix`, then we print the full audit report
// at the end if there's still stuff, because it's silly for `npm audit`
// to tell you to run `npm audit` for details.  otherwise, use the summary
// report.  if we get here, we know it's not quiet or json.
const printAuditReport = (npm, report) => {
  if (!report)
    return

  const reporter = npm.command !== 'audit' ? 'install' : 'detail'
  const defaultAuditLevel = npm.command !== 'audit' ? 'none' : 'low'
  const auditLevel = npm.flatOptions.auditLevel || defaultAuditLevel

  const res = auditReport(report, {
    reporter,
    ...npm.flatOptions,
    auditLevel,
  })
  process.exitCode = process.exitCode || res.exitCode
  output('\n' + res.report)
}

const packagesChangedMessage = (npm, { added, removed, changed, audited }) => {
  const msg = ['\n']
  if (added === 0 && removed === 0 && changed === 0) {
    msg.push('up to date')
    if (audited)
      msg.push(', ')
  } else {
    if (added)
      msg.push(`added ${added} package${added === 1 ? '' : 's'}`)

    if (removed) {
      if (added)
        msg.push(', ')

      if (added && !audited && !changed)
        msg.push('and ')

      msg.push(`removed ${removed} package${removed === 1 ? '' : 's'}`)
    }
    if (changed) {
      if (added || removed)
        msg.push(', ')

      if (!audited && (added || removed))
        msg.push('and ')

      msg.push(`changed ${changed} package${changed === 1 ? '' : 's'}`)
    }
    if (audited)
      msg.push(', and ')
  }
  if (audited)
    msg.push(`audited ${audited} package${audited === 1 ? '' : 's'}`)

  msg.push(` in ${ms(Date.now() - npm.started)}`)
  output(msg.join(''))
}

const packagesFundingMessage = ({ funding }) => {
  if (!funding)
    return

  output('')
  const pkg = funding === 1 ? 'package' : 'packages'
  const is = funding === 1 ? 'is' : 'are'
  output(`${funding} ${pkg} ${is} looking for funding`)
  output('  run `npm fund` for details')
}

module.exports = reifyOutput
