#!/bin/bash
cd "$(dirname "$0")"

echo "Building the dependencies..."

command -v xxd >/dev/null 2>&1 || { echo >&2 "xxd is not installed. (For OS X: https://developer.apple.com/legacy/library/documentation/Darwin/Reference/ManPages/man1/xxd.1.html)"; exit 1; }

xxd -i ./html/configuration.html > ./build/configuration.c

echo "Dependencies are build"

