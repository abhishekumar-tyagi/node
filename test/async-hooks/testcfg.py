import sys, os
sys.path.append(os.path.join(os.path.dirname(__file__), '..'))
import testpy

def GetConfiguration(context, root):
  return testpy.AsyncHooksTestConfiguration(context, root, 'async-hooks')
