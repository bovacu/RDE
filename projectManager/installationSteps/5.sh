GDE_PATH="$1"
cd "$GDE_PATH"/GDE

mkdir libs/SDL_net
cd submodules/SDL_net && ./autogen.sh && ./configure --prefix="$FOLDER"/libs/SDL_net && make && make install && cd ../..