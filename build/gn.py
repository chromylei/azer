#!/bin/bash
# -*- coding: utf-8 -*-  

import os
import sys
import subprocess

gnpath="build/tools/win32/gn"
build_type = "dbg"

def EscapeStringForGN(s):
  """Converts a string to a GN string literal."""
  # Escape $ characters which have special meaning to GN.
  return '"' + s.replace('$', '\\$').replace('"', '\\"') + '"'

def GetGNArgsString(gnargs):
  gn_args = '';
  default_args = [
      ('component_mode', 'shared_library'),
  ]

  for i in default_args:
    if i[0] not in gnargs:
      gn_args += ' ' + i[0] + '=' + EscapeStringForGN(i[1]);

  for key in gnargs.keys():
    gn_args += ' ' + key + '=' + EscapeStringForGN(gnargs[key]);

  return gn_args;

if __name__ == '__main__':
  root = os.path.dirname(sys.argv[0])
  root = os.path.normpath(root + "/../")
  os.chdir(root)
  gnargs = {}

  # 尽量不要制定 root, 否则会造成路径错误
  dbg_args = [os.path.normpath(root + "/" + gnpath),
      'gen', '-q',
      '--args=is_debug=true', # + GetGNArgsString(gnargs),
      '--output=' + os.path.normpath(root + '/out/dbg'),
      ]

  print " ".join(dbg_args)
  subprocess.call(dbg_args)

  opt_args = [os.path.normpath(root + "/" + gnpath),
      'gen', '-q',
      '--args=is_debug=false', # + GetGNArgsString(gnargs),
      '--output=' + os.path.normpath(root + '/out/opt'),
      ]

  print " ".join(opt_args)
  subprocess.call(opt_args)
