cd build
clang++ ../engine/src/rde.cpp -shared -fPIC -DRDE_AUDIO_MODULE -DRDE_PHYSICS_MODULE -I../engine/include -I../external -I../external/linux/include -L../external/linux/lib -lm -lSDL2 -lglad -o libRDE.so
cd ..