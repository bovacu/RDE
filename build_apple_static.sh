clang -c -g -O0 -std=c99 -mmacosx-version-min=$(sw_vers -productVersion) engine/src/rde.c -DRDE_AUDIO_MODULE -DRDE_PHYSICS_MODULE -I./engine/include -I./engine/src -I./external -I./external/osx_x86_64/include -Werror -Wall -Wextra -Wno-tautological-constant-out-of-range-compare -o build/libRDE.o
libtool -static -o build/libRDE.a build/libRDE.o
rm build/libRDE.o