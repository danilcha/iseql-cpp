#!/bin/bash

set -e

source ./common.inc.sh


build()
(
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    cmake -DCMAKE_BUILD_TYPE=Release "$BUILD_DIR_SOURCE" "$@"
    make -j 4 iseql
)



switchToRelease
build

switchToCounters
build -DCOUNTERS=ON

switchToEBI
build -DEBI=ON
