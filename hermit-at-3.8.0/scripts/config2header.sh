#!/bin/sh

usage() {
    echo "$0 INPUT_FILE OUTPUT_FILE"
    echo ""
    echo "INPUT_FILE:"
    echo "  eg. .config"
    echo "OUTPUT_FILE:"
    echo "  eg. autoconf.h"
}

if [ ! -e "$1" ]; then
    echo file not found: $1
    exit 1
fi

if [ "$2" = "" ]; then
    usage
    exit 1
fi

cat $1 | \
    sed -e "s/^#/\/\//" | \
    sed -e "s/^\/\/ CONFIG/#undef CONFIG/" | \
    sed -e "s/is not set//" | \
    sed -e "s/^CONFIG/#define CONFIG/" | \
    sed -e "s/=y/ 1/" | \
    sed -e "s/=/ /" \
    > $2

