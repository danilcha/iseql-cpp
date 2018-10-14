#!/bin/bash

DATA_DIR=~/data

SCRIPT_DIR=`dirname "$0"`


BUILD_DIR=""
EXE=""
BUILD_DIR_SOURCE="../.."

switchToRelease()
{
    BUILD_DIR="target/release"
    EXE="$BUILD_DIR/src/iseql"
}


switchToCounters()
{
    BUILD_DIR="target/counters"
    EXE="$BUILD_DIR/src/iseql"
}


switchToEBI()
{
    BUILD_DIR="target/ebi"
    EXE="$BUILD_DIR/src/iseql"
}


switchToRelease
