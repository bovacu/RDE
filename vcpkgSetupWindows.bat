git clone https://github.com/microsoft/vcpkg.git
cd vcpkg && bootstrap-vcpkg.bat && cd ..
vcpkg/vcpkg.exe install zlib:x64-windows
vcpkg/vcpkg.exe install libpng:x64-windows
vcpkg/vcpkg.exe install libjpeg-turbo:x64-windows
vcpkg/vcpkg.exe install libvorbis:x64-windows
vcpkg/vcpkg.exe install mpg123:x64-windows
vcpkg/vcpkg.exe install box2d:x64-windows
vcpkg/vcpkg.exe install entt:x64-windows
vcpkg/vcpkg.exe install toml11:x64-windows
vcpkg/vcpkg.exe install freetype[core]:x64-windows
vcpkg/vcpkg.exe install sdl2[core,x11]:x64-windows
vcpkg/vcpkg.exe install sdl2-image[core,libjpeg-turbo]:x64-windows
vcpkg/vcpkg.exe install sdl2-mixer[mpg123,libvorbis]:x64-windows
vcpkg/vcpkg.exe install glad:x64-windows
vcpkg/vcpkg.exe install imgui[docking-experimental,opengl3-binding,sdl2-binding]:x64-windows
vcpkg/vcpkg.exe install glm:x64-windows
vcpkg/vcpkg.exe install nlohmann-json:x64-windows