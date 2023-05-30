sudo apt-get install pkg-config
sudo apt-get install zip
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg 
./bootstrap-vcpkg.sh
./vcpkg/vcpkg install zlib:x64-linux &&
./vcpkg/vcpkg install libpng:x64-linux &&
./vcpkg/vcpkg install libjpeg-turbo:x64-linux &&
./vcpkg/vcpkg install libvorbis:x64-linux &&
./vcpkg/vcpkg install mpg123:x64-linux &&
./vcpkg/vcpkg install entt:x64-linux &&
#vcpkg/vcpkg install freetype[core]:x64-linux
./vcpkg/vcpkg install sdl2[x11,wayland]:x64-linux &&
./vcpkg/vcpkg install sdl2-image[libjpeg-turbo]:x64-linux &&
./vcpkg/vcpkg install sdl2-mixer[core,mpg123,libvorbis]:x64-linux &&
./vcpkg/vcpkg install glad:x64-linux &&
./vcpkg/vcpkg install imgui[docking-experimental,opengl3-binding,sdl2-binding]:x64-linux &&
./vcpkg/vcpkg install glm:x64-linux &&
./vcpkg/vcpkg install nlohmann-json:x64-linux
