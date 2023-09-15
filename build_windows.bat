@echo off
cd build

@echo on
clang -g -O0 -std=c99 ..\engine\src\rde.c -shared -DRDE_AUDIO_MODULE -DRDE_PHYSICS_MODULE -I../engine/include -I../engine/src -I../external -I../external/windows/include -L../external/windows/lib -L../external/windows/lib/manual-link -lSDL2main -lSDL2 -lglad -lcglm -Werror -Wall -Wextra -Wno-tautological-constant-out-of-range-compare -o RDE.dll
@echo off

cd ..
