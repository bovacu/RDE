clang++ -c -g -O0 engine/src/rde.cpp -DRDE_AUDIO_MODULE -DRDE_PHYSICS_MODULE -I./engine/include -I./engine/src -I./external -I./external/linux/include -ldl -lm -lpthread -Werror -Wall -Wextra -Wno-tautological-constant-out-of-range-compare -o build/libRDE.o
ar r build/libRDE.o build/libRDE.a
rm build/libRDE.o