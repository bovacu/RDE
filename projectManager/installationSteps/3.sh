GDE_PATH="$1"
cd "$GDE_PATH"/GDE

FOLDER="$PWD"

mkdir "$GDE_PATH"/GDE/libs/SDL_image
cd submodules/SDL_image && ./autogen.sh && ./configure --prefix="$FOLDER"/libs/SDL_image && make && make install && cd ../..