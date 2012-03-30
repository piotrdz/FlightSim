#!/bin/bash

LANGS="en_US pl_PL"
PACKAGE=FlightSim

set -x

for L in $LANGS; do
  if [ -e $L.mo ]; then
    mkdir -p ../data/tr/$L/LC_MESSAGES
    cp -f $L.mo ../data/tr/$L/LC_MESSAGES/$PACKAGE.mo
  fi
done
