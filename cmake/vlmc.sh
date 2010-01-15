#!/bin/sh

BIN="bin/launcher"
if [ -f $BIN ]; then
    exec $BIN "$@"
else
    echo "VLMC not build, read INSTALL first"
fi
