sudo apt-get install build-essential cmake clang git autoconf
git submodule init
git submodule update
cd submodules/imgui && git checkout docking
cd ../..

sed -i -e 's/add_library(jpeg STATIC)/add_library(jpeg SHARED)/g' submodules/SDL_image/external/jpeg-9d/CMakeLists.txt

cd submodules/SDL && ./autogen && ./configure && make && sudo make install && cd ../..
cd submodules/SDL_image && ./autogen && ./configure && make && sudo make install && cd ../..
cd submodules/SDL_mixer && ./autogen && ./configure && make && sudo make install && cd ../..
cd submodules/SDL_net && ./autogen && ./configure && make && sudo make install && cd ../..
cd submodules/freetype && ./autogen && ./autogen && ./configure && make && sudo make install && cd ../..

