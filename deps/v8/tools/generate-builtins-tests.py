#!/usr/bin/env python3
# Copyright 2014 the V8 project authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

# for py2/py3 compatibility
from __future__ import print_function

import json
import optparse
import os
import random
import shutil
import subprocess
import sys


SKIPLIST = [
  # Skip special d8 functions.
  "load", "os", "print", "read", "readline", "quit"
]


def GetRandomObject():
  return random.choice([
    "0", "1", "2.5", "0x1000", "\"string\"", "{foo: \"bar\"}", "[1, 2, 3]",
    "function() { return 0; }"
  ])


g_var_index = 0


def GetVars(result, num, first = []):
  global g_var_index
  variables = []
  for i in range(num):
    variables.append("__v_%d" % g_var_index)
    g_var_index += 1
  for var in variables:
    result.append("var %s = %s;" % (var, GetRandomObject()))
  return ", ".join(first + variables)


# Wraps |string| in try..catch.
def TryCatch(result, string, exception_behavior = ""):
  result.append("try { %s } catch(e) { %s }" % (string, exception_behavior))


def BuildTests(function, full_name, options):
  assert function["type"] == "function"
  global g_var_index
  g_var_index = 0
  result = ["// AUTO-GENERATED BY tools/generate-builtins-tests.py.\n"]
  result.append("// Function call test:")
  length = function["length"]
  TryCatch(result, "%s(%s);" % (full_name, GetVars(result, length)))

  if "prototype" in function:
    proto = function["prototype"]
    result.append("\n// Constructor test:")
    TryCatch(result,
             "var recv = new %s(%s);" % (full_name, GetVars(result, length)),
             "var recv = new Object();")

    getters = []
    methods = []
    for prop in proto:
      proto_property = proto[prop]
      proto_property_type = proto_property["type"]
      if proto_property_type == "getter":
        getters.append(proto_property)
        result.append("recv.__defineGetter__(\"%s\", "
                      "function() { return %s; });" %
                      (proto_property["name"], GetVars(result, 1)))
      if proto_property_type == "number":
        result.append("recv.__defineGetter__(\"%s\", "
                      "function() { return %s; });" %
                      (proto_property["name"], GetVars(result, 1)))
      if proto_property_type == "function":
        methods.append(proto_property)
    if getters:
      result.append("\n// Getter tests:")
      for getter in getters:
        result.append("print(recv.%s);" % getter["name"])
    if methods:
      result.append("\n// Method tests:")
      for method in methods:
        args = GetVars(result, method["length"], ["recv"])
        call = "%s.prototype.%s.call(%s)" % (full_name, method["name"], args)
        TryCatch(result, call)

  filename = os.path.join(options.outdir, "%s.js" % (full_name))
  with open(filename, "w") as f:
    f.write("\n".join(result))
    f.write("\n")


def VisitObject(obj, path, options):
  obj_type = obj["type"]
  obj_name = "%s%s" % (path, obj["name"])
  if obj_type == "function":
    BuildTests(obj, obj_name, options)
  if "properties" in obj:
    for prop_name in obj["properties"]:
      prop = obj["properties"][prop_name]
      VisitObject(prop, "%s." % (obj_name), options)


def ClearGeneratedFiles(options):
  if os.path.exists(options.outdir):
    shutil.rmtree(options.outdir)


def GenerateTests(options):
  ClearGeneratedFiles(options)  # Re-generate everything.
  output = subprocess.check_output(
      "%s %s" % (options.d8, options.script), shell=True).strip()
  objects = json.loads(output)

  os.makedirs(options.outdir)
  for obj_name in objects:
    if obj_name in SKIPLIST: continue
    obj = objects[obj_name]
    VisitObject(obj, "", options)


def BuildOptions():
  result = optparse.OptionParser()
  result.add_option("--d8", help="d8 binary to use",
                    default="out/ia32.release/d8")
  result.add_option("--outdir", help="directory where to place generated tests",
                    default="test/mjsunit/builtins-gen")
  result.add_option("--script", help="builtins detector script to run in d8",
                    default="tools/detect-builtins.js")
  return result


def Main():
  parser = BuildOptions()
  (options, args) = parser.parse_args()
  if len(args) != 1 or args[0] == "help":
    parser.print_help()
    return 1
  action = args[0]

  if action == "generate":
    GenerateTests(options)
    return 0

  if action == "clear":
    ClearGeneratedFiles(options)
    return 0

  print("Unknown action: %s" % action)
  parser.print_help()
  return 1


if __name__ == "__main__":
  sys.exit(Main())
