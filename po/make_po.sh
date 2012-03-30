#!/bin/bash
# Making or updating .po files

LANGS="en_US pl_PL"
INPUT="flightsim.pot"

set -x

for L in $LANGS; do
  if ! [ -e $L.po ]; then
    msginit --no-translator --locale $L --output-file $L.po --input $INPUT
  else
    msgmerge --output-file $L.po $L.po $INPUT
  fi
done
