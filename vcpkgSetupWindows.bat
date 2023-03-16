git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
bootstrap-vcpkg.bat
vcpkg.exe install zlib:x64-windows
vcpkg.exe install libpng:x64-windows
vcpkg.exe install libjpeg-turbo:x64-windows
vcpkg.exe install libvorbis:x64-windows
vcpkg.exe install mpg123:x64-windows
vcpkg.exe install entt:x64-windows
vcpkg.exe install chipmunk:x64-windows
vcpkg.exe install freetype:x64-windows
vcpkg.exe install sdl2:x64-windows
vcpkg.exe install sdl2-image[libjpeg-turbo]:x64-windows
vcpkg.exe install sdl2-mixer:x64-windows
vcpkg.exe install glad:x64-windows
vcpkg.exe install imgui[docking-experimental,opengl3-binding,sdl2-binding]:x64-windows
vcpkg.exe install imguizmo:x64-windows
vcpkg.exe install glm:x64-windows
vcpkg.exe install nlohmann-json:x64-windows
