GDE_PATH="$1"
cd "$GDE_PATH"/GDE

cd submodules/freetype && ./autogen.sh && ./configure && make && sudo make install && cd ../..