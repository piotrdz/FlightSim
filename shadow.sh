#!/bin/bash

if [ $# -ne 2 ]; then
  echo "Usage: $0 dir shadow_dir"
  exit 1
fi

# A script to shadow directory structure in build/ so that
# build and CMake objects can be deleted easily

TEMP_DIR=/tmp/shadow.$RANDOM
SRC_DIR=`readlink -f "$1"`

echo "Shadowing structure in temp: $TEMP_DIR"
find "$1" -type d -printf "mkdir -p '$TEMP_DIR/%P'\n" -o -type f -printf "ln -s '$SRC_DIR/%P' '$TEMP_DIR/%P'\n" | sh


echo "Moving temp: $TEMP_DIR to $2"
mv $TEMP_DIR "$2"
