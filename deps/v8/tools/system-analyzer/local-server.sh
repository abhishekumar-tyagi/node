#!/bin/bash

# Copyright 2022 the V8 project authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

for i in "$@"; do
  case $i in
    -h|--help)
    echo "Starts a local server for V8's system anaylizer"
    echo "It's accessible http://localhost:8000"
    echo "Note: The server also exposes local binary information via 'nm'"
    exit;
  ;;
    *)
    echo "Invalid option: $i"
    exit 1;
  ;;
  esac
done

if ! command -v ws &> /dev/null
then
    echo "'ws' not found!"
    echo "Please install local-web-server:"
    echo "npm install local-web-server"
    echo ""
    exit
fi

TOOLS_DIR=`readlink "$0"` || TOOLS_DIR="$0";
TOOLS_DIR=`dirname "$TOOLS_DIR"`;
cd "$TOOLS_DIR/.."
TOOLS_DIR=`pwd -P`

if lsof -t -i TCP:8000; then
  echo "locahost:8000 is already in use. You can kill it with:"
  echo "lsof -t -i TCP:8000 | xargs kill"
  exit 1
fi

echo "Starting local symbol server"
ws --stack $TOOLS_DIR/system-analyzer/lws-middleware.js lws-static lws-index & PID=$!

# Kill server after 1h
for i in `seq 3600`; do
  if ps -p $PID > /dev/null 2>&1; then
    sleep 1;
  fi
done

if ps -p $PID > /dev/null 2>&1; then
  echo "Automatically killing the local server after timeout"
  kill $PID
fi
