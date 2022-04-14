const t = require('tap')
const { load: loadMockNpm } = require('../../fixtures/mock-npm')
const MockRegistry = require('../../fixtures/mock-registry.js')

const username = 'foo'
const auth = { '//registry.npmjs.org/:_authToken': 'test-auth-token' }

t.test('npm whoami', async (t) => {
  const { npm, joinedOutput } = await loadMockNpm(t, { config: auth })
  const registry = new MockRegistry({
    tap: t,
    registry: npm.config.get('registry'),
    authorization: 'test-auth-token',
  })
  registry.whoami({ username })
  await npm.exec('whoami', [])
  t.equal(joinedOutput(), username, 'should print username')
})

t.test('npm whoami --json', async (t) => {
  const { npm, joinedOutput } = await loadMockNpm(t, {
    config: {
      json: true,
      ...auth,
    },
  })
  const registry = new MockRegistry({
    tap: t,
    registry: npm.config.get('registry'),
    authorization: 'test-auth-token',
  })
  registry.whoami({ username })
  await npm.exec('whoami', [])
  t.equal(JSON.parse(joinedOutput()), username, 'should print username')
})
