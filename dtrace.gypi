{
  'conditions': [
    [ 'node_use_dtrace=="true"', {
      'defines': [ 'HAVE_DTRACE=1' ],
      'include_dirs': [ '<(SHARED_INTERMEDIATE_DIR)' ],
      #
      # DTrace is supported on linux, solaris, mac, and bsd.  There are
      # three object files associated with DTrace support, but they're
      # not all used all the time:
      #
      #   node_dtrace.o           all configurations
      #   node_dtrace_ustack.o    not supported on mac and linux
      #   node_dtrace_provider.o  All except OS X.  "dtrace -G" is not
      #                           used on OS X.
      #
      # Note that node_dtrace_provider.cc and node_dtrace_ustack.cc do not
      # actually exist.  They're listed here to trick GYP into linking the
      # corresponding object files into the final "node" executable.  These
      # object files are generated by "dtrace -G" using custom actions
      # below, and the GYP-generated Makefiles will properly build them when
      # needed.
      #
      'sources': [ 'src/node_dtrace.cc' ],
      'conditions': [
        [ 'OS=="linux"', {
          'sources': [
            '<(SHARED_INTERMEDIATE_DIR)/node_dtrace_header/node_dtrace_provider.o'
          ],
        }],
        [ 'OS!="mac" and OS!="linux"', {
          'sources': [
            'src/node_dtrace_ustack.cc',
            'src/node_dtrace_provider.cc',
          ],
        }],
      ],
    }],
    [ 'node_use_dtrace=="true" and OS!="linux"', {
      'actions': [
        {
          'action_name': 'node_dtrace_header',
          'inputs': [ 'src/node_provider.d' ],
          'outputs': [ '<(SHARED_INTERMEDIATE_DIR)/node_dtrace_header/node_provider.h' ],
          'action': [ 'dtrace', '-h', '-xnolibs', '-s', '<@(_inputs)',
                      '-o', '<@(_outputs)' ]
        }
      ]
    } ],
    [ 'node_use_dtrace=="true" and OS=="linux"', {
      'actions': [
        {
          'action_name': 'node_dtrace_header',
          'inputs': [ 'src/node_provider.d' ],
          'outputs': [ '<(SHARED_INTERMEDIATE_DIR)/node_dtrace_header/node_provider.h' ],
          'action': [ 'dtrace', '-h', '-s', '<@(_inputs)', '-o', '<@(_outputs)' ]
        },
      ],
    }],
    [ 'node_use_dtrace=="true" and OS!="mac" and OS!="linux"', {
      'actions': [
        {
          'action_name': 'node_dtrace_provider_o',
          'inputs': [
            '<(obj_dir)/node_base/src/node_dtrace.o',
          ],
          'outputs': [
            '<(obj_dir)/node_base/src/node_dtrace_provider.o'
          ],
          'action': [
            'dtrace',
            '-G', '-xnolibs',
            '-s', 'src/node_provider.d',
            '<@(_inputs)',
            '-o', '<@(_outputs)',
          ]
        }
      ]
    }],
    [ 'node_use_dtrace=="true" and OS=="linux"', {
      'actions': [
        {
          'action_name': 'node_dtrace_provider_o',
          'inputs': [ 'src/node_provider.d' ],
          'outputs': [
            '<(SHARED_INTERMEDIATE_DIR)/node_dtrace_header/node_dtrace_provider.o'
          ],
          'action': [
            'dtrace', '-C', '-G', '-s', '<@(_inputs)', '-o', '<@(_outputs)'
          ],
        },
      ],
    }],
    [ 'node_use_dtrace=="true" and OS!="mac" and OS!="linux"', {
      'actions': [
        {
          'action_name': 'node_dtrace_ustack_constants',
          'inputs': [
            '<(v8_base)'
          ],
          'outputs': [
            '<(SHARED_INTERMEDIATE_DIR)/node_dtrace_header/v8constants.h'
          ],
          'action': [
            'tools/genv8constants.py',
            '<@(_outputs)',
            '<@(_inputs)'
          ]
        },
        {
          'action_name': 'node_dtrace_ustack',
          'inputs': [
            'src/v8ustack.d',
            '<(SHARED_INTERMEDIATE_DIR)/node_dtrace_header/v8constants.h'
          ],
          'outputs': [
            '<(obj_dir)/<(node_lib_target_name)/src/node_dtrace_ustack.o'
          ],
          'conditions': [
            [ 'target_arch=="ia32" or target_arch=="arm"', {
              'action': [
                'dtrace',
                '-32',
                '-I<(SHARED_INTERMEDIATE_DIR)/node_dtrace_header',
                '-Isrc',
                '-C', '-G',
                '-s', 'src/v8ustack.d',
                '-o', '<@(_outputs)',
              ],
            }],
            [ 'target_arch=="x64"', {
              'action': [
                'dtrace',
                '-64',
                '-I<(SHARED_INTERMEDIATE_DIR)/node_dtrace_header',
                '-Isrc',
                '-C', '-G',
                '-s', 'src/v8ustack.d',
                '-o', '<@(_outputs)',
              ],
            }],
          ],
        },
      ],
    }],
    [ 'node_use_dtrace=="true"', {
      'actions': [
        {
          'action_name': 'specialize_node_d',
          'inputs': [
            'src/node.d'
          ],
          'outputs': [
            '<(PRODUCT_DIR)/node.d',
          ],
          'action': [
            'tools/specialize_node_d.py',
            '<@(_outputs)',
            '<@(_inputs)',
            '<@(OS)',
            '<@(target_arch)',
          ],
        },
      ],
    }],
  ]
}
