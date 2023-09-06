cd build
clang++ -g -O0 ../engine/src/rde.cpp -shared -fPIC -DRDE_AUDIO_MODULE -DRDE_PHYSICS_MODULE -I../engine/include -I../engine/src -I../external -I../external/linux/include -L../external/linux/lib -ldl -lm -lpthread -lSDL2 -lglad -Werror -Wall -Wextra -Wno-tautological-constant-out-of-range-compare -o libRDE.so
cd ..