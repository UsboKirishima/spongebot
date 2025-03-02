# TODO: check dependencies
rm -rf build
mkdir build
gcc -std=c99 -Wall -DDEBUG $(find . -name "*.c") -o build/patrick
./build/patrick
