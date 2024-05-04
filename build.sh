#!/bin/bash

mkdir -p ./build
gcc -I /usr/local/include/SDL2 -o ./build/main ./src/main.cpp -lSDL2