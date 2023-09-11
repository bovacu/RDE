clang++ -c -g -O0 -mmacosx-version-min=$(sw_vers -productVersion) -std=c++11 engine/src/rde.cpp -DRDE_AUDIO_MODULE -DRDE_PHYSICS_MODULE -I./engine/include -I./engine/src -I./external -I./external/osx_x86_64/include -Werror -Wall -Wextra -Wno-tautological-constant-out-of-range-compare -o build/libRDE.o
libtool -static -o build/libRDE.a build/libRDE.o
rm build/libRDE.o