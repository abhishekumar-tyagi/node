# Keeping this separate since Node.js does use it
{
  'target_name': 'v8_external_snapshot',
  'type': 'static_library',
  'variables': {
    'libraries_extras_bin_file': '<(SHARED_INTERMEDIATE_DIR)/libraries-extras.bin',
  },
  'conditions': [
    ['v8_use_external_startup_data==1', {
      'conditions': [
        ['want_separate_host_toolset==1', {
          'toolsets': ['host', 'target'],
          'dependencies': [
            'mksnapshot#host',
          ]}, {
           'toolsets': ['target'],
           'dependencies': [
             'mksnapshot',
           ],
         }],
        ['component=="shared_library"', {
          'defines': [
            'BUILDING_V8_SHARED',
          ],
          'direct_dependent_settings': {
            'defines': [
              'USING_V8_SHARED',
            ],
          },
        }],
      ],
      'dependencies': [
        'v8_base',
      ],
      'include_dirs': [
        '..',
        '<(DEPTH)',
      ],
      'sources': [
        '../src/setup-isolate-deserialize.cc',
        '../src/snapshot/embedded-empty.cc',
        '../src/snapshot/natives-external.cc',
        '../src/snapshot/snapshot-external.cc',
        '<(embedded_builtins_snapshot_src)',
      ],
      'actions': [
        {
          'action_name': 'js2c_extras_bin',
          'inputs': [
            '../tools/js2c.py',
            '<@(v8_extra_library_files)',
          ],
          'outputs': ['<@(libraries_extras_bin_file)'],
          'action': [
            'python',
            '../tools/js2c.py',
            '<(SHARED_INTERMEDIATE_DIR)/extras-libraries.cc',
            'EXTRAS',
            '<@(v8_extra_library_files)',
            '--startup_blob', '<@(libraries_extras_bin_file)',
            '--nojs',
          ],
        },
        {
          'action_name': 'concatenate_natives_blob',
          'inputs': [
            '../tools/concatenate-files.py',
            '<(SHARED_INTERMEDIATE_DIR)/libraries-extras.bin',
          ],
          'conditions': [
            ['want_separate_host_toolset==1', {
              'target_conditions': [
                ['_toolset=="host"', {
                  'outputs': [
                    '<(PRODUCT_DIR)/natives_blob_host.bin',
                  ],
                  'action': [
                    'python', '<@(_inputs)', '<(PRODUCT_DIR)/natives_blob_host.bin'
                  ],
                }, {
                   'outputs': [
                     '<(PRODUCT_DIR)/natives_blob.bin',
                   ],
                   'action': [
                     'python', '<@(_inputs)', '<(PRODUCT_DIR)/natives_blob.bin'
                   ],
                 }],
              ],
            }, {
               'outputs': [
                 '<(PRODUCT_DIR)/natives_blob.bin',
               ],
               'action': [
                 'python', '<@(_inputs)', '<(PRODUCT_DIR)/natives_blob.bin'
               ],
             }],
          ],
        },
        {
          'action_name': 'run_mksnapshot (external)',
          'inputs': [
            '<(mksnapshot_exec)',
          ],
          'variables': {
            'mksnapshot_flags': [],
            'conditions': [
              ['v8_random_seed!=0', {
                'mksnapshot_flags': ['--random-seed', '<(v8_random_seed)'],
              }],
              ['v8_vector_stores!=0', {
                'mksnapshot_flags': ['--vector-stores'],
              }],
              ['v8_os_page_size!=0', {
                'mksnapshot_flags': ['--v8_os_page_size', '<(v8_os_page_size)'],
              }],
              ['v8_enable_embedded_builtins="true"', {
                # 'embedded_builtins_snapshot_src': [ "$target_gen_dir/embedded${suffix}.cc" ],
                # 'mksnapshot_flags':  ["--embedded_src", "$target_gen_dir/embedded${suffix}.cc",],
                # if (invoker.embedded_variant != "") {
                #   args += [
                #     "--embedded_variant",
                #     invoker.embedded_variant,
                #   ]
                # }
              },
               ],
            ],
          },
          'conditions': [
            ['v8_embed_script!=""', {
              'inputs': [
                '<(v8_embed_script)',
              ],
            }],
            ['want_separate_host_toolset==1', {
              'target_conditions': [
                ['_toolset=="host"', {
                  'outputs': [
                    '<(PRODUCT_DIR)/snapshot_blob_host.bin',
                    '<(embedded_builtins_snapshot_src)'
                  ],
                  'action': [
                    '<(mksnapshot_exec)',
                    '<@(mksnapshot_flags)',
                    '--startup_blob', '<(PRODUCT_DIR)/snapshot_blob_host.bin',
                    '<(embed_script)',
                    '<(warmup_script)',
                  ],
                }, {
                  'outputs': [
                    '<(PRODUCT_DIR)/snapshot_blob.bin',
                  ],
                  'action': [
                    '<(mksnapshot_exec)',
                    '<@(mksnapshot_flags)',
                    '--startup_blob', '<(PRODUCT_DIR)/snapshot_blob.bin',
                    '<(embed_script)',
                    '<(warmup_script)',
                  ],
                }],
              ],
            }, {
              'outputs': [
                '<(PRODUCT_DIR)/snapshot_blob.bin',
                '<(embedded_builtins_snapshot_src)'
              ],
              'action': [
                '<(mksnapshot_exec)',
                '<@(mksnapshot_flags)',
                '--startup_blob', '<(PRODUCT_DIR)/snapshot_blob.bin',
                '<(embed_script)',
                '<(warmup_script)',
              ],
            }],
          ],
        },
      ],
    }],
  ],
}
