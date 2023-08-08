{
  'target_name': 'openssl-cli',
  'type': 'executable',
  'dependencies': ['openssl'],
  'defines': [
    'MONOLITH'
  ],
  'includes': ['openssl.gypi'],
  'sources': ['<@(openssl_cli_sources)'],
  'conditions': [
    ['OS=="solaris"', {
      'libraries': ['<@(openssl_cli_libraries_solaris)']
    }, 'OS=="win"', {
      'link_settings': {
        'libraries': ['<@(openssl_cli_libraries_win)'],
      },
    }, 'OS in "linux android"', {
      'link_settings': {
        'libraries': [
          '-ldl',
        ],
      },
    }],
    # Avoid excessive LTO
    ['enable_lto=="true"', {
      'ldflags': [ '-fno-lto' ],
    }],
    # Avoid excessive PGO
    ['enable_pgo_generate=="true"', {
      'cflags': [ '-fno-profile' ],
      'cflags!': [ '-fprofile-generate' ],
      'ldflags': [ '-fno-profile' ],
      'ldflags!': [ '-fprofile-generate' ],
    }, ],
    ['enable_pgo_use=="true"', {
      'cflags': [ '-fno-profile' ],
      'cflags!': [ '-fprofile-use -fprofile-correction' ],
      'ldflags': [ '-fno-profile' ],
      'ldflags!': [ '-fprofile-use -fprofile-correction' ],
    }, ],
  ],
}
