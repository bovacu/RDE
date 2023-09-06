cd build
clang++ -g -O0 ../engine/src/rde.cpp -DRDE_AUDIO_MODULE -DRDE_PHYSICS_MODULE -I../engine/include -I../engine/src -I../external -I../external/linux/include -ldl -lm -lpthread -Werror -Wall -Wextra -Wno-tautological-constant-out-of-range-compare -o libRDE.o
ar r libRDE.o libRDE.a
rm libRDE.o
cd ..