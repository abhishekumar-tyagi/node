#!/usr/bin/env python

# Copyright (c) 2014 Google Inc. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

"""
Make sure macro expansion of $(TargetPath) is handled.
"""

import TestGyp

import sys

if sys.platform == 'win32':
  test = TestGyp.TestGyp(formats=['msvs', 'ninja'])

  CHDIR = 'vs-macros'
  test.run_gyp('targetpath.gyp', chdir=CHDIR)
  test.build('targetpath.gyp', test.ALL, chdir=CHDIR)
  test.built_file_must_exist('test_targetpath_executable.exe', chdir=CHDIR)
  test.built_file_must_exist('test_targetpath_loadable_module.dll',
                             chdir=CHDIR)
  test.built_file_must_exist('test_targetpath_shared_library.dll',
                             chdir=CHDIR)
  test.built_file_must_exist('test_targetpath_static_library.lib',
                             chdir=CHDIR)
  test.built_file_must_exist('test_targetpath_product_extension.foo',
                             chdir=CHDIR)
  test.pass_test()
