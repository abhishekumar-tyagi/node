const Arborist = require('@npmcli/arborist')
const auditReport = require('npm-audit-report')
const reifyFinish = require('./utils/reify-finish.js')
const auditError = require('./utils/audit-error.js')
const BaseCommand = require('./base-command.js')

class Audit extends BaseCommand {
  /* istanbul ignore next - see test/lib/load-all-commands.js */
  static get description () {
    return 'Run a security audit'
  }

  /* istanbul ignore next - see test/lib/load-all-commands.js */
  static get name () {
    return 'audit'
  }

  /* istanbul ignore next - see test/lib/load-all-commands.js */
  static get params () {
    return [
      'dry-run',
      'force',
      'json',
      'package-lock-only',
      'production',
    ]
  }

  /* istanbul ignore next - see test/lib/load-all-commands.js */
  static get usage () {
    return ['[fix]']
  }

  async completion (opts) {
    const argv = opts.conf.argv.remain

    if (argv.length === 2)
      return ['fix']

    switch (argv[2]) {
      case 'fix':
        return []
      default:
        throw new Error(argv[2] + ' not recognized')
    }
  }

  exec (args, cb) {
    this.audit(args).then(() => cb()).catch(cb)
  }

  async audit (args) {
    const reporter = this.npm.config.get('json') ? 'json' : 'detail'
    const opts = {
      ...this.npm.flatOptions,
      audit: true,
      path: this.npm.prefix,
      reporter,
    }
    const arb = new Arborist(opts)
    const fix = args[0] === 'fix'
    await arb.audit({ fix })
    if (fix)
      await reifyFinish(this.npm, arb)
    else {
      // will throw if there's an error, because this is an audit command
      auditError(this.npm, arb.auditReport)
      const result = auditReport(arb.auditReport, opts)
      process.exitCode = process.exitCode || result.exitCode
      this.npm.output(result.report)
    }
  }
}

module.exports = Audit
