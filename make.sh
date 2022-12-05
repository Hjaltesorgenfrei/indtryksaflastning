#!/bin/bash
g++ main.cpp -o indtryk.exe -lSDL2
emcc --bind main.cpp -o indtryk.html -s USE_SDL=2 -s ALLOW_MEMORY_GROWTH=1