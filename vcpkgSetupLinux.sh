sudo apt-get install pkg-config
sudo apt-get install zip
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg 
./bootstrap-vcpkg.sh
./vcpkg install zlib:x64-linux
./vcpkg install libpng:x64-linux
./vcpkg install libjpeg-turbo:x64-linux
./vcpkg install libvorbis:x64-linux
./vcpkg install mpg123:x64-linux
./vcpkg install entt:x64-linux
#vcpkg/vcpkg install freetype[core]:x64-linux
./vcpkg install sdl2[x11,wayland]:x64-linux
./vcpkg install sdl2-image[core,libjpeg-turbo]:x64-linux
./vcpkg install sdl2-mixer[core,mpg123,libvorbis]:x64-linux
./vcpkg install glad:x64-linux
./vcpkg install imgui[docking-experimental,opengl3-binding,sdl2-binding]:x64-linux
./vcpkg install glm:x64-linux
./vcpkg install nlohmann-json:x64-linux