{
  'target_name': 'openssl-cli',
  'type': 'executable',
  'dependencies': ['openssl'],
  'defines': [
    'MONOLITH'
  ],
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
  ],
}
