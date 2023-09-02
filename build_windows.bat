@echo off
cd build

@echo on
clang++ ..\engine\src\rde.cpp -shared -DRDE_AUDIO_MODULE -DRDE_PHYSICS_MODULE -I../engine/include -I../engine/src -I../external -I../external/windows/include -L../external/windows/lib -L../external/windows/lib/manual-link -lSDL2main -lSDL2 -lSDL2_image -lglad -Werror -Wall -Wextra -o RDE.dll
@echo off

cd ..