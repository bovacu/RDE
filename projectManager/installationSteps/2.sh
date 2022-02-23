GDE_PATH="$1"
cd "$GDE_PATH"/GDE

FOLDER="$PWD"

mkdir "$GDE_PATH"/GDE/libs && mkdir "$GDE_PATH"/GDE/libs/SDL
cd submodules/SDL && ./autogen.sh && ./configure --prefix="$FOLDER"/libs/SDL && make && make install && cd ../..