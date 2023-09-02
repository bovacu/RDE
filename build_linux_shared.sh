cd build
clang++ ../engine/src/rde.cpp -shared -fPIC -DRDE_AUDIO_MODULE -DRDE_PHYSICS_MODULE -I../engine/include -I../engine/src -I../external -I../external/linux/include -L../external/linux/lib -lm -lSDL2 -lglad -Werror -Wall -Wextra -o libRDE.so
cd ..