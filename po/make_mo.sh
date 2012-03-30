#!/bin/bash

LANGS="en_US pl_PL"

set -x

for L in $LANGS; do
  if [ -e $L.po ]; then
    msgfmt --check --verbose --output-file $L.mo $L.po
  fi
done
