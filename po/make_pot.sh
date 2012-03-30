#!/bin/bash
# Making .pot, smoking .pot ;)

PACKAGE="FlightSim"
PACKAGE_VERSION="1.0"
INPUT=`find ../src/ \( -name '*.cpp' -o -name '*.h' \) -printf '%p '`
OUTPUT="flightsim.pot"

set -x

xgettext --keyword=_ --from-code='UTF-8' --package-name $PACKAGE --package-version $PACKAGE_VERSION --default-domain $PACKAGE --output $OUTPUT $INPUT