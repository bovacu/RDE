sudo apt-get install build-essential
sudo apt-get install cmake
sudo apt-get install clang

cmake -Blibs/SDL/build -Slibs/SDL
cd libs/SDL/build && make
cd libs/SDL/build && make install

cmake -Blibs/SDL_image/build -Slibs/SDL_image
cd libs/SDL_image/build && make
cd libs/SDL_image/build && make install

cmake -Blibs/SDL_net/build -Slibs/SDL_net
cd libs/SDL_net/build && make
cd libs/SDL_net/build && make install

cmake -Blibs/SDL_mixer/build -Slibs/SDL_mixer
cd libs/SDL_mixer/build && make
cd libs/SDL_mixer/build && make install

