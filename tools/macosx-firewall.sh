#!/bin/bash
# Script that adds rules to Mac OS X Socket Firewall to avoid
# popups asking to accept incoming network connections when
# running tests.
SFW="/usr/libexec/ApplicationFirewall/socketfilterfw"
TOOLSDIR="`dirname \"$0\"`"
TOOLSDIR="`( cd \"$TOOLSDIR\" && pwd) `"
ROOTDIR="`( cd \"$TOOLSDIR/..\" && pwd) `"
OUTDIR="$TOOLSDIR/../out"
OUTDIR="`( cd \"$OUTDIR\" && pwd) `"
NODE_RELEASE="$OUTDIR/Release/node"
NODE_DEBUG="$OUTDIR/Debug/node"
NODE_LINK="$ROOTDIR/node"

if [ -f $SFW ];
then
  # Duplicating these commands on purpose as the symbolic link node might be
  # linked to either out/Debug/node or out/Release/node depending on the
  # BUILDTYPE.
  $SFW --remove "$NODE_DEBUG"
  $SFW --remove "$NODE_DEBUG"
  $SFW --remove "$NODE_RELEASE"
  $SFW --remove "$NODE_RELEASE"
  $SFW --remove "$NODE_LINK"

  $SFW --add "$NODE_DEBUG"
  $SFW --add "$NODE_RELEASE"
  $SFW --add "$NODE_LINK"

  $SFW --unblock "$NODE_DEBUG"
  $SFW --unblock "$NODE_RELEASE"
  $SFW --unblock "$NODE_LINK"
else
  echo "SocketFirewall not found in location: $SFW"
fi
