#!/bin/bash

mkdir -p build
C_FLAGS="-std=c11 -Wall -Werror -pedantic"
INCLUDES="-I src"
# Build static lib (ckit)
clang -c -o build/ckit.o $C_FLAGS $INCLUDES src/mem.c 

# Build example executable
clang -c -o build/example.o $C_FLAGS $INCLUDES examples/example.c 
clang -o build/example build/ckit.o build/example.o 