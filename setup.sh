sudo apt-get install build-essential cmake clang git autoconf libtool m4 automake xorg-dev
git submodule init
git submodule update
cd submodules/imgui && git checkout docking

cd ../.. && sed -i -e 's/add_library(jpeg STATIC)/add_library(jpeg SHARED)/g' submodules/SDL_image/external/jpeg-9d/CMakeLists.txt

cd submodules/SDL && ./autogen.sh && ./configure && make && sudo make install && cd ../..
cd submodules/SDL_image && ./autogen.sh && ./configure && make && sudo make install && cd ../..
cd submodules/SDL_mixer && ./autogen.sh && ./configure && make && sudo make install && cd ../..
cd submodules/SDL_net && ./autogen.sh && ./configure && make && sudo make install && cd ../..
cd submodules/freetype && ./autogen.sh && ./configure && make && sudo make install && cd ../..

