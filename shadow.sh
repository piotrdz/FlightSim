#!/bin/bash

# A script to shadow directory structure in build/ so that
# build and CMake objects can be deleted easily

if [ -e build ]; then
  echo "Removing old build/"
  rm -rf build/
fi

TEMP_DIR=/tmp/build.$RANDOM

echo "Shadowing structure in temp: $TEMP_DIR"
CURRENT_DIR=`pwd`
find . -type d -printf "mkdir -p '$TEMP_DIR/%P'\n" -o -type f -printf "ln -s '$CURRENT_DIR/%P' '$TEMP_DIR/%P'\n" | sh


echo "Moving temp: $TEMP_DIR to build/"
mv $TEMP_DIR build/
