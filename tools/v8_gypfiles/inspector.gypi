# Copyright 2016 the V8 project authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'variables': {
    'inspector_protocol_path': '<(V8_ROOT)/third_party/inspector_protocol',
    'inspector_protocol_files': [
      '<(inspector_protocol_path)/lib/base_string_adapter_cc.template',
      '<(inspector_protocol_path)/lib/base_string_adapter_h.template',
      '<(inspector_protocol_path)/lib/DispatcherBase_cpp.template',
      '<(inspector_protocol_path)/lib/DispatcherBase_h.template',
      '<(inspector_protocol_path)/lib/ErrorSupport_cpp.template',
      '<(inspector_protocol_path)/lib/ErrorSupport_h.template',
      '<(inspector_protocol_path)/lib/Forward_h.template',
      '<(inspector_protocol_path)/lib/FrontendChannel_h.template',
      '<(inspector_protocol_path)/lib/Object_cpp.template',
      '<(inspector_protocol_path)/lib/Object_h.template',
      '<(inspector_protocol_path)/lib/Parser_cpp.template',
      '<(inspector_protocol_path)/lib/Parser_h.template',
      '<(inspector_protocol_path)/lib/Protocol_cpp.template',
      '<(inspector_protocol_path)/lib/ValueConversions_h.template',
      '<(inspector_protocol_path)/lib/Values_cpp.template',
      '<(inspector_protocol_path)/lib/Values_h.template',
      '<(inspector_protocol_path)/templates/Exported_h.template',
      '<(inspector_protocol_path)/templates/Imported_h.template',
      '<(inspector_protocol_path)/templates/TypeBuilder_cpp.template',
      '<(inspector_protocol_path)/templates/TypeBuilder_h.template',
      '<(inspector_protocol_path)/code_generator.py',
    ],
    'inspector_path': '<(V8_ROOT)/src/inspector',
    'inspector_generated_output_root': '<(SHARED_INTERMEDIATE_DIR)/inspector-generated-output-root',
    'inspector_generated_sources': [
      '<(inspector_generated_output_root)/src/inspector/protocol/Forward.h',
      '<(inspector_generated_output_root)/src/inspector/protocol/Protocol.cpp',
      '<(inspector_generated_output_root)/src/inspector/protocol/Protocol.h',
      '<(inspector_generated_output_root)/src/inspector/protocol/Console.cpp',
      '<(inspector_generated_output_root)/src/inspector/protocol/Console.h',
      '<(inspector_generated_output_root)/src/inspector/protocol/Debugger.cpp',
      '<(inspector_generated_output_root)/src/inspector/protocol/Debugger.h',
      '<(inspector_generated_output_root)/src/inspector/protocol/HeapProfiler.cpp',
      '<(inspector_generated_output_root)/src/inspector/protocol/HeapProfiler.h',
      '<(inspector_generated_output_root)/src/inspector/protocol/Profiler.cpp',
      '<(inspector_generated_output_root)/src/inspector/protocol/Profiler.h',
      '<(inspector_generated_output_root)/src/inspector/protocol/Runtime.cpp',
      '<(inspector_generated_output_root)/src/inspector/protocol/Runtime.h',
      '<(inspector_generated_output_root)/src/inspector/protocol/Schema.cpp',
      '<(inspector_generated_output_root)/src/inspector/protocol/Schema.h',
      '<(inspector_generated_output_root)/include/inspector/Debugger.h',
      '<(inspector_generated_output_root)/include/inspector/Runtime.h',
      '<(inspector_generated_output_root)/include/inspector/Schema.h',
    ],

    'inspector_all_sources': [
      '<(V8_ROOT)/include/v8-inspector.h',
      '<(V8_ROOT)/include/v8-inspector-protocol.h',
      '<(V8_ROOT)/src/inspector/custom-preview.cc',
      '<(V8_ROOT)/src/inspector/custom-preview.h',
      '<(V8_ROOT)/src/inspector/injected-script.cc',
      '<(V8_ROOT)/src/inspector/injected-script.h',
      '<(V8_ROOT)/src/inspector/inspected-context.cc',
      '<(V8_ROOT)/src/inspector/inspected-context.h',
      '<(V8_ROOT)/src/inspector/remote-object-id.cc',
      '<(V8_ROOT)/src/inspector/remote-object-id.h',
      '<(V8_ROOT)/src/inspector/search-util.cc',
      '<(V8_ROOT)/src/inspector/search-util.h',
      '<(V8_ROOT)/src/inspector/string-16.cc',
      '<(V8_ROOT)/src/inspector/string-16.h',
      '<(V8_ROOT)/src/inspector/string-util.cc',
      '<(V8_ROOT)/src/inspector/string-util.h',
      '<(V8_ROOT)/src/inspector/test-interface.cc',
      '<(V8_ROOT)/src/inspector/test-interface.h',
      '<(V8_ROOT)/src/inspector/v8-console.cc',
      '<(V8_ROOT)/src/inspector/v8-console.h',
      '<(V8_ROOT)/src/inspector/v8-console-agent-impl.cc',
      '<(V8_ROOT)/src/inspector/v8-console-agent-impl.h',
      '<(V8_ROOT)/src/inspector/v8-console-message.cc',
      '<(V8_ROOT)/src/inspector/v8-console-message.h',
      '<(V8_ROOT)/src/inspector/v8-debugger.cc',
      '<(V8_ROOT)/src/inspector/v8-debugger.h',
      '<(V8_ROOT)/src/inspector/v8-debugger-agent-impl.cc',
      '<(V8_ROOT)/src/inspector/v8-debugger-agent-impl.h',
      '<(V8_ROOT)/src/inspector/v8-debugger-script.cc',
      '<(V8_ROOT)/src/inspector/v8-debugger-script.h',
      '<(V8_ROOT)/src/inspector/v8-heap-profiler-agent-impl.cc',
      '<(V8_ROOT)/src/inspector/v8-heap-profiler-agent-impl.h',
      '<(V8_ROOT)/src/inspector/v8-inspector-impl.cc',
      '<(V8_ROOT)/src/inspector/v8-inspector-impl.h',
      '<(V8_ROOT)/src/inspector/v8-inspector-protocol-encoding.cc',
      '<(V8_ROOT)/src/inspector/v8-inspector-protocol-encoding.h',
      '<(V8_ROOT)/src/inspector/v8-inspector-session-impl.cc',
      '<(V8_ROOT)/src/inspector/v8-inspector-session-impl.h',
      '<(V8_ROOT)/src/inspector/v8-profiler-agent-impl.cc',
      '<(V8_ROOT)/src/inspector/v8-profiler-agent-impl.h',
      '<(V8_ROOT)/src/inspector/v8-regex.cc',
      '<(V8_ROOT)/src/inspector/v8-regex.h',
      '<(V8_ROOT)/src/inspector/v8-runtime-agent-impl.cc',
      '<(V8_ROOT)/src/inspector/v8-runtime-agent-impl.h',
      '<(V8_ROOT)/src/inspector/v8-schema-agent-impl.cc',
      '<(V8_ROOT)/src/inspector/v8-schema-agent-impl.h',
      '<(V8_ROOT)/src/inspector/v8-stack-trace-impl.cc',
      '<(V8_ROOT)/src/inspector/v8-stack-trace-impl.h',
      '<(V8_ROOT)/src/inspector/v8-value-utils.cc',
      '<(V8_ROOT)/src/inspector/v8-value-utils.h',
      '<(V8_ROOT)/src/inspector/value-mirror.cc',
      '<(V8_ROOT)/src/inspector/value-mirror.h',
      '<(V8_ROOT)/src/inspector/wasm-translation.cc',
      '<(V8_ROOT)/src/inspector/wasm-translation.h',
      # Flat merge `third_party/inspector_protocol:inspector_string_conversions`
      '<(inspector_path)/v8-string-conversions.cc',
      '<(inspector_path)/v8-string-conversions.h',
      # Flat merge `third_party/inspector_protocol:encoding`
      '<(inspector_protocol_path)/encoding/encoding.cc',
      '<(inspector_protocol_path)/encoding/encoding.h',
      # Flat merge `third_party/inspector_protocol:bindings`
      '<(inspector_protocol_path)/bindings/bindings.cc',
      '<(inspector_protocol_path)/bindings/bindings.h',

    ],
    'v8_inspector_js_protocol': '<(V8_ROOT)/include/js_protocol.pdl',
  },
  'include_dirs': [
    '<(inspector_generated_output_root)',
    '<(inspector_protocol_path)',
  ],
  'actions': [
    {
      'action_name': 'protocol_compatibility',
      'inputs': [
        '<(v8_inspector_js_protocol)',
      ],
      'outputs': [
        '<@(inspector_generated_output_root)/src/js_protocol.stamp',
      ],
      'action': [
        'python',
        '<(inspector_protocol_path)/check_protocol_compatibility.py',
        '--stamp', '<@(_outputs)',
        '<@(_inputs)',
      ],
      'message': 'Checking inspector protocol compatibility',
    },
    {
      'action_name': 'protocol_generated_sources',
      'inputs': [
        '<(v8_inspector_js_protocol)',
        '<(inspector_path)/inspector_protocol_config.json',
        '<@(inspector_protocol_files)',
      ],
      'outputs': [
        '<@(inspector_generated_sources)',
      ],
      'process_outputs_as_sources': 1,
      'action': [
        'python',
        '<(inspector_protocol_path)/code_generator.py',
        '--jinja_dir', '<(V8_ROOT)/third_party',
        '--output_base', '<(inspector_generated_output_root)/src/inspector',
        '--config', '<(inspector_path)/inspector_protocol_config.json',
        '--inspector_protocol_dir', '<(inspector_protocol_path)',
      ],
      'message': 'Generating inspector protocol sources from protocol json',
    },
  ],
}
