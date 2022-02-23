GDE_PATH="$1"
cd "$GDE_PATH"/GDE

FOLDER="$PWD"

mkdir "$GDE_PATH"/GDE/libs/SDL_net
cd submodules/SDL_net && ./autogen.sh && ./configure --prefix="$FOLDER"/libs/SDL_net && make && make install && cd ../..