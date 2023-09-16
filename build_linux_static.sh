clang -c -g -O0 -std=c99 engine/src/rde.c -DRDE_AUDIO_MODULE -DRDE_PHYSICS_MODULE -I./engine/include -I./engine/src -I./external/include -Werror -Wall -Wextra -Wno-tautological-constant-out-of-range-compare -o build/libRDE.o
ar -crs build/libRDE.a build/libRDE.o
rm build/libRDE.o