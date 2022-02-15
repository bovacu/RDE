sudo apt-get install build-essential
sudo apt-get install cmake
sudo apt-get install clang

cmake -Blibs/SDL/build -Slibs/SDL
cd libs/SDL/build && make
sudo make install

cd ../../..
cmake -Blibs/SDL_mixer/build -Slibs/SDL_mixer
cd libs/SDL_mixer/build && make
sudo make install

cd ../../..
cmake -Blibs/SDL_image/build -Slibs/SDL_image
echo "$PWD"
cd libs/SDL_image/build && make
sudo make install

