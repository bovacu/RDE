GDE_PATH="$1"
cd "$GDE_PATH"/GDE

mkdir libs/SDL_mixer
cd submodules/SDL_mixer && ./autogen.sh && ./configure --prefix="$FOLDER"/libs/SDL_mixer && make && make install && cd ../..