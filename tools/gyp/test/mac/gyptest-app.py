#!/usr/bin/env python

# Copyright (c) 2012 Google Inc. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

"""
Verifies that app bundles are built correctly.
"""

from __future__ import print_function

import os
import plistlib
import subprocess
import sys

import TestGyp
from XCodeDetect import XCodeDetect

test = TestGyp.TestGyp(formats=['ninja', 'make', 'xcode'], platforms=['darwin'], disable="This test is currently disabled: https://crbug.com/483696.")


# noinspection PyUnreachableCode
def CheckFileXMLPropertyList(file):
  output = subprocess.check_output(['file', file])
  # The double space after XML is intentional.
  if not 'XML  document text' in output:
    print('File: Expected XML  document text, got %s' % output)
    test.fail_test()


def ExpectEq(expected, actual):
  if expected != actual:
    print('Expected "%s", got "%s"' % (expected, actual), file=sys.stderr)
    test.fail_test()


def ls(path):
  """
  Returns a list of all files in a directory, relative to the directory.
  """
  result = []
  for dirpath, _, files in os.walk(path):
    for f in files:
      result.append(os.path.join(dirpath, f)[len(path) + 1:])
  return result


test.run_gyp('test.gyp', chdir='app-bundle')

test.build('test.gyp', test.ALL, chdir='app-bundle')

# Binary
test.built_file_must_exist('Test App Gyp.app/Contents/MacOS/Test App Gyp', chdir='app-bundle')

# Info.plist
info_plist = test.built_file_path('Test App Gyp.app/Contents/Info.plist', chdir='app-bundle')
test.must_exist(info_plist)
test.must_contain(info_plist, 'com.google.Test-App-Gyp')  # Variable expansion
test.must_not_contain(info_plist, '${MACOSX_DEPLOYMENT_TARGET}')
CheckFileXMLPropertyList(info_plist)

if test.format != 'make':
  # TODO: Synthesized plist entries aren't hooked up in the make generator.
  machine = subprocess.check_output(['sw_vers', '-buildVersion']).rstrip('\n')
  plist = plistlib.readPlist(info_plist)
  ExpectEq(machine, plist['BuildMachineOSBuild'])

  # Prior to Xcode 5.0.0, SDKROOT (and thus DTSDKName) was only defined if
  # set in the Xcode project file. Starting with that version, it is always
  # defined.
  expected = ''
  if XCodeDetect.Version() >= '0500':
    version = XCodeDetect.SDKVersion()
    expected = 'macosx' + version
  ExpectEq(expected, plist['DTSDKName'])
  ExpectEq(XCodeDetect.Version(), plist['DTXcode'])

# Resources
strings_files = ['InfoPlist.strings', 'utf-16be.strings', 'utf-16le.strings']
for f in strings_files:
  strings = test.built_file_path(os.path.join('Test App Gyp.app/Contents/Resources/English.lproj', f), chdir='app-bundle')
  test.must_exist(strings)
  # Xcodes writes UTF-16LE with BOM.
  contents = open(strings, 'rb').read()
  if not contents.startswith(('\xff\xfe' + '/* Localized').encode('utf-16le')):
    test.fail_test()

test.built_file_must_exist('Test App Gyp.app/Contents/Resources/English.lproj/MainMenu.nib', chdir='app-bundle')

# Packaging
test.built_file_must_exist('Test App Gyp.app/Contents/PkgInfo', chdir='app-bundle')
test.built_file_must_match('Test App Gyp.app/Contents/PkgInfo', 'APPLause', chdir='app-bundle')

# Check that no other files get added to the bundle.
expected_file_set = set(
  [
    'Contents/MacOS/Test App Gyp',
    'Contents/Info.plist',
    'Contents/Resources/English.lproj/MainMenu.nib',
    'Contents/PkgInfo',
  ] + [os.path.join('Contents/Resources/English.lproj', f) for f in strings_files]
)
if set(ls(test.built_file_path('Test App Gyp.app', chdir='app-bundle'))) != expected_file_set:
  test.fail_test()

test.pass_test()
