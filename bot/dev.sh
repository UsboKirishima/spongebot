#!/bin/bash

# TODO: check dependencies


rm -rf build
mkdir build

if [ "$1" == "safe" ]; then
    gcc -std=c99 -Iinclude -Wall $(find . -name "*.c") -o build/bot
else
    gcc -std=c99 -Iinclude -Wall -DDEBUG $(find . -name "*.c") -o build/bot
fi

./build/bot
