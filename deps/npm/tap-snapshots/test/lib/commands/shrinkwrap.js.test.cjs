/* IMPORTANT
 * This snapshot file is auto-generated, but designed for humans.
 * It should be checked into source control and tracked carefully.
 * Re-generate by setting TAP_SNAPSHOT=1 and running tests.
 * Make sure to inspect the output below.  Do not ignore changes!
 */
'use strict'
exports[`test/lib/commands/shrinkwrap.js TAP with hidden lockfile ancient > must match snapshot 1`] = `
{
  "localPrefix": {
    "node_modules": {
      ".package-lock.json": {
        "lockfileVersion": 1
      }
    }
  },
  "config": {},
  "shrinkwrap": {
    "name": "tap-testdir-shrinkwrap-with-hidden-lockfile-ancient",
    "lockfileVersion": 1,
    "requires": true
  },
  "logs": [
    "created a lockfile as npm-shrinkwrap.json"
  ]
}
`

exports[`test/lib/commands/shrinkwrap.js TAP with hidden lockfile ancient upgrade > must match snapshot 1`] = `
{
  "localPrefix": {
    "node_modules": {
      ".package-lock.json": {
        "lockfileVersion": 1
      }
    }
  },
  "config": {
    "lockfileVersion": 3
  },
  "shrinkwrap": {
    "name": "tap-testdir-shrinkwrap-with-hidden-lockfile-ancient-upgrade",
    "lockfileVersion": 3,
    "requires": true,
    "packages": {}
  },
  "logs": [
    "created a lockfile as npm-shrinkwrap.json with version 3"
  ]
}
`

exports[`test/lib/commands/shrinkwrap.js TAP with hidden lockfile existing > must match snapshot 1`] = `
{
  "localPrefix": {
    "node_modules": {
      ".package-lock.json": {
        "lockfileVersion": 2
      }
    }
  },
  "config": {},
  "shrinkwrap": {
    "name": "tap-testdir-shrinkwrap-with-hidden-lockfile-existing",
    "lockfileVersion": 2,
    "requires": true,
    "packages": {}
  },
  "logs": [
    "created a lockfile as npm-shrinkwrap.json"
  ]
}
`

exports[`test/lib/commands/shrinkwrap.js TAP with hidden lockfile existing downgrade > must match snapshot 1`] = `
{
  "localPrefix": {
    "node_modules": {
      ".package-lock.json": {
        "lockfileVersion": 2
      }
    }
  },
  "config": {
    "lockfileVersion": 1
  },
  "shrinkwrap": {
    "name": "tap-testdir-shrinkwrap-with-hidden-lockfile-existing-downgrade",
    "lockfileVersion": 1,
    "requires": true
  },
  "logs": [
    "created a lockfile as npm-shrinkwrap.json with version 1"
  ]
}
`

exports[`test/lib/commands/shrinkwrap.js TAP with hidden lockfile existing upgrade > must match snapshot 1`] = `
{
  "localPrefix": {
    "node_modules": {
      ".package-lock.json": {
        "lockfileVersion": 2
      }
    }
  },
  "config": {
    "lockfileVersion": 3
  },
  "shrinkwrap": {
    "name": "tap-testdir-shrinkwrap-with-hidden-lockfile-existing-upgrade",
    "lockfileVersion": 3,
    "requires": true,
    "packages": {}
  },
  "logs": [
    "created a lockfile as npm-shrinkwrap.json with version 3"
  ]
}
`

exports[`test/lib/commands/shrinkwrap.js TAP with nothing ancient > must match snapshot 1`] = `
{
  "localPrefix": {},
  "config": {},
  "shrinkwrap": {
    "name": "tap-testdir-shrinkwrap-with-nothing-ancient",
    "lockfileVersion": 2,
    "requires": true,
    "packages": {}
  },
  "logs": [
    "created a lockfile as npm-shrinkwrap.json with version 2"
  ]
}
`

exports[`test/lib/commands/shrinkwrap.js TAP with nothing ancient upgrade > must match snapshot 1`] = `
{
  "localPrefix": {},
  "config": {
    "lockfileVersion": 3
  },
  "shrinkwrap": {
    "name": "tap-testdir-shrinkwrap-with-nothing-ancient-upgrade",
    "lockfileVersion": 3,
    "requires": true,
    "packages": {}
  },
  "logs": [
    "created a lockfile as npm-shrinkwrap.json with version 3"
  ]
}
`

exports[`test/lib/commands/shrinkwrap.js TAP with npm-shrinkwrap.json ancient > must match snapshot 1`] = `
{
  "localPrefix": {
    "npm-shrinkwrap.json": {
      "lockfileVersion": 1
    }
  },
  "config": {},
  "shrinkwrap": {
    "name": "tap-testdir-shrinkwrap-with-npm-shrinkwrap.json-ancient",
    "lockfileVersion": 2,
    "requires": true,
    "packages": {
      "": {
        "name": "tap-testdir-shrinkwrap-with-npm-shrinkwrap.json-ancient"
      }
    }
  },
  "logs": [
    "npm-shrinkwrap.json updated to version 2"
  ]
}
`

exports[`test/lib/commands/shrinkwrap.js TAP with npm-shrinkwrap.json ancient upgrade > must match snapshot 1`] = `
{
  "localPrefix": {
    "npm-shrinkwrap.json": {
      "lockfileVersion": 1
    }
  },
  "config": {
    "lockfileVersion": 3
  },
  "shrinkwrap": {
    "name": "tap-testdir-shrinkwrap-with-npm-shrinkwrap.json-ancient-upgrade",
    "lockfileVersion": 3,
    "requires": true,
    "packages": {
      "": {
        "name": "tap-testdir-shrinkwrap-with-npm-shrinkwrap.json-ancient-upgrade"
      }
    }
  },
  "logs": [
    "npm-shrinkwrap.json updated to version 3"
  ]
}
`

exports[`test/lib/commands/shrinkwrap.js TAP with npm-shrinkwrap.json existing > must match snapshot 1`] = `
{
  "localPrefix": {
    "npm-shrinkwrap.json": {
      "lockfileVersion": 2
    }
  },
  "config": {},
  "shrinkwrap": {
    "name": "tap-testdir-shrinkwrap-with-npm-shrinkwrap.json-existing",
    "lockfileVersion": 2,
    "requires": true,
    "packages": {
      "": {
        "name": "tap-testdir-shrinkwrap-with-npm-shrinkwrap.json-existing"
      }
    }
  },
  "logs": [
    "npm-shrinkwrap.json up to date"
  ]
}
`

exports[`test/lib/commands/shrinkwrap.js TAP with npm-shrinkwrap.json existing downgrade > must match snapshot 1`] = `
{
  "localPrefix": {
    "npm-shrinkwrap.json": {
      "lockfileVersion": 2
    }
  },
  "config": {
    "lockfileVersion": 1
  },
  "shrinkwrap": {
    "name": "tap-testdir-shrinkwrap-with-npm-shrinkwrap.json-existing-downgrade",
    "lockfileVersion": 1,
    "requires": true
  },
  "logs": [
    "npm-shrinkwrap.json updated to version 1"
  ]
}
`

exports[`test/lib/commands/shrinkwrap.js TAP with npm-shrinkwrap.json existing upgrade > must match snapshot 1`] = `
{
  "localPrefix": {
    "npm-shrinkwrap.json": {
      "lockfileVersion": 2
    }
  },
  "config": {
    "lockfileVersion": 3
  },
  "shrinkwrap": {
    "name": "tap-testdir-shrinkwrap-with-npm-shrinkwrap.json-existing-upgrade",
    "lockfileVersion": 3,
    "requires": true,
    "packages": {
      "": {
        "name": "tap-testdir-shrinkwrap-with-npm-shrinkwrap.json-existing-upgrade"
      }
    }
  },
  "logs": [
    "npm-shrinkwrap.json updated to version 3"
  ]
}
`

exports[`test/lib/commands/shrinkwrap.js TAP with package-lock.json ancient > must match snapshot 1`] = `
{
  "localPrefix": {
    "package-lock.json": {
      "lockfileVersion": 1
    }
  },
  "config": {},
  "shrinkwrap": {
    "name": "tap-testdir-shrinkwrap-with-package-lock.json-ancient",
    "lockfileVersion": 2,
    "requires": true,
    "packages": {
      "": {
        "name": "tap-testdir-shrinkwrap-with-package-lock.json-ancient"
      }
    }
  },
  "logs": [
    "package-lock.json has been renamed to npm-shrinkwrap.json and updated to version 2"
  ]
}
`

exports[`test/lib/commands/shrinkwrap.js TAP with package-lock.json ancient upgrade > must match snapshot 1`] = `
{
  "localPrefix": {
    "package-lock.json": {
      "lockfileVersion": 1
    }
  },
  "config": {
    "lockfileVersion": 3
  },
  "shrinkwrap": {
    "name": "tap-testdir-shrinkwrap-with-package-lock.json-ancient-upgrade",
    "lockfileVersion": 3,
    "requires": true,
    "packages": {
      "": {
        "name": "tap-testdir-shrinkwrap-with-package-lock.json-ancient-upgrade"
      }
    }
  },
  "logs": [
    "package-lock.json has been renamed to npm-shrinkwrap.json and updated to version 3"
  ]
}
`

exports[`test/lib/commands/shrinkwrap.js TAP with package-lock.json existing > must match snapshot 1`] = `
{
  "localPrefix": {
    "package-lock.json": {
      "lockfileVersion": 2
    }
  },
  "config": {},
  "shrinkwrap": {
    "name": "tap-testdir-shrinkwrap-with-package-lock.json-existing",
    "lockfileVersion": 2,
    "requires": true,
    "packages": {
      "": {
        "name": "tap-testdir-shrinkwrap-with-package-lock.json-existing"
      }
    }
  },
  "logs": [
    "package-lock.json has been renamed to npm-shrinkwrap.json"
  ]
}
`

exports[`test/lib/commands/shrinkwrap.js TAP with package-lock.json existing downgrade > must match snapshot 1`] = `
{
  "localPrefix": {
    "package-lock.json": {
      "lockfileVersion": 2
    }
  },
  "config": {
    "lockfileVersion": 1
  },
  "shrinkwrap": {
    "name": "tap-testdir-shrinkwrap-with-package-lock.json-existing-downgrade",
    "lockfileVersion": 1,
    "requires": true
  },
  "logs": [
    "package-lock.json has been renamed to npm-shrinkwrap.json and updated to version 1"
  ]
}
`

exports[`test/lib/commands/shrinkwrap.js TAP with package-lock.json existing upgrade > must match snapshot 1`] = `
{
  "localPrefix": {
    "package-lock.json": {
      "lockfileVersion": 2
    }
  },
  "config": {
    "lockfileVersion": 3
  },
  "shrinkwrap": {
    "name": "tap-testdir-shrinkwrap-with-package-lock.json-existing-upgrade",
    "lockfileVersion": 3,
    "requires": true,
    "packages": {
      "": {
        "name": "tap-testdir-shrinkwrap-with-package-lock.json-existing-upgrade"
      }
    }
  },
  "logs": [
    "package-lock.json has been renamed to npm-shrinkwrap.json and updated to version 3"
  ]
}
`
