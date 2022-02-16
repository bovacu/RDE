sudo apt-get install build-essential
sudo apt-get install cmake
sudo apt-get install clang
sudo apt-get install git

cmake -Blibs/SDL/build -Slibs/SDL -DBUILD_SHARED_LIBS=FALSE
cd libs/SDL/build && make
sudo make install

cd ../../..
cd libs/SDL_mixer && ./configure
make
sudo make install

cd ../../..
cmake -Blibs/SDL_image/build -Slibs/SDL_image -DBUILD_SHARED_LIBS=FALSE
cd libs/SDL_image/build && make
sudo make install

