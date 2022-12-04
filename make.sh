#!/bin/bash
g++ main.cpp -o indtryk.exe -lSDL2 -lSDL2_gfx
emcc --bind main.cpp -o indtryk.html -s USE_SDL=2 -s USE_SDL_GFX=2 -s ALLOW_MEMORY_GROWTH=1