clang -g -O0 -std=c99 -mmacosx-version-min=$(sw_vers -productVersion) engine/src/rde.c -shared -fPIC -DRDE_AUDIO_MODULE -DRDE_PHYSICS_MODULE -I./engine/include -I./engine/src -I./external/include -L./external/libs/osx_x86_64 -ldl -lm -lpthread -lSDL2main -lSDL2 -lglad -lcglm -Werror -Wall -Wextra -Wno-tautological-constant-out-of-range-compare -o build/libRDE.dylib