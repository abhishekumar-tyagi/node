/* IMPORTANT
 * This snapshot file is auto-generated, but designed for humans.
 * It should be checked into source control and tracked carefully.
 * Re-generate by setting TAP_SNAPSHOT=1 and running tests.
 * Make sure to inspect the output below.  Do not ignore changes!
 */
'use strict'
exports[`test/lib/commands/audit.js TAP audit fix - bulk endpoint > lockfile has test-dep-a@1.0.1 1`] = `
{
  "name": "test-dep",
  "version": "1.0.0",
  "lockfileVersion": 2,
  "requires": true,
  "packages": {
    "": {
      "name": "test-dep",
      "version": "1.0.0",
      "dependencies": {
        "test-dep-a": "*"
      }
    },
    "node_modules/test-dep-a": {
      "version": "1.0.1",
      "resolved": "https://registry.npmjs.org/test-dep-a/-/test-dep-a-1.0.1.tgz"
    }
  },
  "dependencies": {
    "test-dep-a": {
      "version": "1.0.1",
      "resolved": "https://registry.npmjs.org/test-dep-a/-/test-dep-a-1.0.1.tgz"
    }
  }
}

`

exports[`test/lib/commands/audit.js TAP audit fix - bulk endpoint > must match snapshot 1`] = `

added 1 package, and audited 2 packages in xxx

found 0 vulnerabilities
`

exports[`test/lib/commands/audit.js TAP audit signatures ignores optional dependencies > must match snapshot 1`] = `
audited 1 package in xxx

1 package has a verified registry signature

`

exports[`test/lib/commands/audit.js TAP audit signatures json output with invalid and missing signatures > must match snapshot 1`] = `
{
  "invalid": [
    {
      "name": "kms-demo",
      "version": "1.0.0",
      "location": "node_modules/kms-demo",
      "resolved": "https://registry.npmjs.org/kms-demo/-/kms-demo-1.0.0.tgz",
      "integrity": "sha512-QqZ7VJ/8xPkS9s2IWB7Shj3qTJdcRyeXKbPQnsZjsPEwvutGv0EGeVchPcauoiDFJlGbZMFq5GDCurAGNSghJQ==",
      "signature": "bogus",
      "keyid": "SHA256:jl3bwswu80PjjokCgh0o2w5c2U4LhQAE57gj9cz1kzA"
    }
  ],
  "missing": [
    {
      "name": "async",
      "version": "1.1.1",
      "location": "node_modules/async",
      "resolved": "https://registry.npmjs.org/async/-/async-1.1.1.tgz"
    }
  ]
}
`

exports[`test/lib/commands/audit.js TAP audit signatures json output with invalid signatures > must match snapshot 1`] = `
{
  "invalid": [
    {
      "name": "kms-demo",
      "version": "1.0.0",
      "location": "node_modules/kms-demo",
      "resolved": "https://registry.npmjs.org/kms-demo/-/kms-demo-1.0.0.tgz",
      "integrity": "sha512-QqZ7VJ/8xPkS9s2IWB7Shj3qTJdcRyeXKbPQnsZjsPEwvutGv0EGeVchPcauoiDFJlGbZMFq5GDCurAGNSghJQ==",
      "signature": "bogus",
      "keyid": "SHA256:jl3bwswu80PjjokCgh0o2w5c2U4LhQAE57gj9cz1kzA"
    }
  ],
  "missing": []
}
`

exports[`test/lib/commands/audit.js TAP audit signatures json output with valid signatures > must match snapshot 1`] = `
{
  "invalid": [],
  "missing": []
}
`

exports[`test/lib/commands/audit.js TAP audit signatures multiple registries with keys and signatures > must match snapshot 1`] = `
audited 2 packages in xxx

2 packages have verified registry signatures

`

exports[`test/lib/commands/audit.js TAP audit signatures omit dev dependencies with missing signature > must match snapshot 1`] = `
audited 1 package in xxx

1 package has a verified registry signature

`

exports[`test/lib/commands/audit.js TAP audit signatures output details about missing signatures > must match snapshot 1`] = `
audited 1 package in xxx

1 package has a missing registry signature but the registry is providing signing keys:

kms-demo@1.0.0 (https://registry.npmjs.org/)
`

exports[`test/lib/commands/audit.js TAP audit signatures third-party registry with invalid signatures errors > must match snapshot 1`] = `
audited 1 package in xxx

1 package has an invalid registry signature:

@npmcli/arborist@1.0.14 (https://verdaccio-clone.org)

Someone might have tampered with this package since it was published on the registry!

`

exports[`test/lib/commands/audit.js TAP audit signatures third-party registry with keys and missing signatures errors > must match snapshot 1`] = `
audited 1 package in xxx

1 package has a missing registry signature but the registry is providing signing keys:

@npmcli/arborist@1.0.14 (https://verdaccio-clone.org)
`

exports[`test/lib/commands/audit.js TAP audit signatures third-party registry with keys and signatures > must match snapshot 1`] = `
audited 1 package in xxx

1 package has a verified registry signature

`

exports[`test/lib/commands/audit.js TAP audit signatures with both invalid and missing signatures > must match snapshot 1`] = `
audited 2 packages in xxx

1 package has a missing registry signature but the registry is providing signing keys:

async@1.1.1 (https://registry.npmjs.org/)

1 package has an invalid registry signature:

kms-demo@1.0.0 (https://registry.npmjs.org/)

Someone might have tampered with this package since it was published on the registry!

`

exports[`test/lib/commands/audit.js TAP audit signatures with bundled and peer deps and no signatures > must match snapshot 1`] = `
audited 1 package in xxx

1 package has a verified registry signature

`

exports[`test/lib/commands/audit.js TAP audit signatures with invalid signatures > must match snapshot 1`] = `
audited 1 package in xxx

1 package has an invalid registry signature:

kms-demo@1.0.0 (https://registry.npmjs.org/)

Someone might have tampered with this package since it was published on the registry!

`

exports[`test/lib/commands/audit.js TAP audit signatures with invalid signtaures and color output enabled > must match snapshot 1`] = `
audited 1 package in xxx

1 package has an [1m[31minvalid[39m[22m registry signature:

[31mkms-demo@1.0.0[39m (https://registry.npmjs.org/)

Someone might have tampered with this package since it was published on the registry!

`

exports[`test/lib/commands/audit.js TAP audit signatures with keys but missing signature > must match snapshot 1`] = `
audited 1 package in xxx

1 package has a missing registry signature but the registry is providing signing keys:

kms-demo@1.0.0 (https://registry.npmjs.org/)
`

exports[`test/lib/commands/audit.js TAP audit signatures with multiple invalid signatures > must match snapshot 1`] = `
audited 2 packages in xxx

2 packages have invalid registry signatures:

async@1.1.1 (https://registry.npmjs.org/)
kms-demo@1.0.0 (https://registry.npmjs.org/)

Someone might have tampered with these packages since they where published on the registry!

`

exports[`test/lib/commands/audit.js TAP audit signatures with multiple missing signatures > must match snapshot 1`] = `
audited 2 packages in xxx

2 packages have missing registry signatures but the registry is providing signing keys:

async@1.1.1 (https://registry.npmjs.org/)
kms-demo@1.0.0 (https://registry.npmjs.org/)
`

exports[`test/lib/commands/audit.js TAP audit signatures with multiple valid signatures and one invalid > must match snapshot 1`] = `
audited 3 packages in xxx

2 packages have verified registry signatures

1 package has an invalid registry signature:

node-fetch@1.6.0 (https://registry.npmjs.org/)

Someone might have tampered with this package since it was published on the registry!

`

exports[`test/lib/commands/audit.js TAP audit signatures with valid and missing signatures > must match snapshot 1`] = `
audited 2 packages in xxx

1 package has a verified registry signature

1 package has a missing registry signature but the registry is providing signing keys:

async@1.1.1 (https://registry.npmjs.org/)
`

exports[`test/lib/commands/audit.js TAP audit signatures with valid signatures > must match snapshot 1`] = `
audited 1 package in xxx

1 package has a verified registry signature

`

exports[`test/lib/commands/audit.js TAP audit signatures with valid signatures using alias > must match snapshot 1`] = `
audited 1 package in xxx

1 package has a verified registry signature

`

exports[`test/lib/commands/audit.js TAP audit signatures workspaces verifies registry deps and ignores local workspace deps > must match snapshot 1`] = `
audited 3 packages in xxx

3 packages have verified registry signatures

`

exports[`test/lib/commands/audit.js TAP audit signatures workspaces verifies registry deps when filtering by workspace name > must match snapshot 1`] = `
audited 2 packages in xxx

2 packages have verified registry signatures

`

exports[`test/lib/commands/audit.js TAP fallback audit > must match snapshot 1`] = `
# npm audit report

test-dep-a  1.0.0
Severity: high
Test advisory 100 - https://github.com/advisories/GHSA-100
fix available via \`npm audit fix\`
node_modules/test-dep-a

1 high severity vulnerability

To address all issues, run:
  npm audit fix
`

exports[`test/lib/commands/audit.js TAP json audit > must match snapshot 1`] = `
{
  "auditReportVersion": 2,
  "vulnerabilities": {
    "test-dep-a": {
      "name": "test-dep-a",
      "severity": "high",
      "isDirect": true,
      "via": [
        {
          "source": 100,
          "name": "test-dep-a",
          "dependency": "test-dep-a",
          "title": "Test advisory 100",
          "url": "https://github.com/advisories/GHSA-100",
          "severity": "high",
          "cwe": [
            "cwe-0"
          ],
          "cvss": {
            "score": 0
          },
          "range": "*"
        }
      ],
      "effects": [],
      "range": "*",
      "nodes": [
        "node_modules/test-dep-a"
      ],
      "fixAvailable": false
    }
  },
  "metadata": {
    "vulnerabilities": {
      "info": 0,
      "low": 0,
      "moderate": 0,
      "high": 1,
      "critical": 0,
      "total": 1
    },
    "dependencies": {
      "prod": 2,
      "dev": 0,
      "optional": 0,
      "peer": 0,
      "peerOptional": 0,
      "total": 1
    }
  }
}
`

exports[`test/lib/commands/audit.js TAP normal audit > must match snapshot 1`] = `
# npm audit report

test-dep-a  1.0.0
Severity: high
Test advisory 100 - https://github.com/advisories/GHSA-100
fix available via \`npm audit fix\`
node_modules/test-dep-a

1 high severity vulnerability

To address all issues, run:
  npm audit fix
`
