GDE_PATH="$1"
cd "$GDE_PATH"/GDE

mkdir libs/SDL
cd submodules/SDL && ./autogen.sh && ./configure --prefix="$FOLDER"/libs/SDL && make && make install && cd ../..