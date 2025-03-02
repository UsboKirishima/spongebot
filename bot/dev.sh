# TODO: check dependencies
rm -rf build
mkdir build
gcc -std=c99 -Wall $(find . -name "*.c") -o build/patrick
./build/patrick