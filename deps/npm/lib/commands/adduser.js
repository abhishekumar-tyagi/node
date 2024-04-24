const log = require('../utils/log-shim.js')
const { redactLog: replaceInfo } = require('@npmcli/redact')
const auth = require('../utils/auth.js')

const BaseCommand = require('../base-command.js')

class AddUser extends BaseCommand {
  static description = 'Add a registry user account'
  static name = 'adduser'
  static params = [
    'registry',
    'scope',
    'auth-type',
  ]

  async exec (args) {
    const scope = this.npm.config.get('scope')
    let registry = this.npm.config.get('registry')

    if (scope) {
      const scopedRegistry = this.npm.config.get(`${scope}:registry`)
      const cliRegistry = this.npm.config.get('registry', 'cli')
      if (scopedRegistry && !cliRegistry) {
        registry = scopedRegistry
      }
    }

    const creds = this.npm.config.getCredentialsByURI(registry)

    log.disableProgress()
    log.notice('', `Log in on ${replaceInfo(registry)}`)

    const { message, newCreds } = await auth.adduser(this.npm, {
      ...this.npm.flatOptions,
      creds,
      registry,
    })

    this.npm.config.delete('_token', 'user') // prevent legacy pollution
    this.npm.config.setCredentialsByURI(registry, newCreds)

    if (scope) {
      this.npm.config.set(scope + ':registry', registry, 'user')
    }

    await this.npm.config.save('user')

    this.npm.output(message)
  }
}
module.exports = AddUser
