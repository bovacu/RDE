sudo apt-get install build-essential cmake clang git autoconf libtool m4 automake xorg-dev mesa-common-dev libgl1-mesa-dev libglu1-mesa-dev mesa-utils libsamplerate-dev libudev-dev libdbus-1-dev libibus-1.0-dev fcitx-libs-dev zlib1g bzip2 libpng-dev
git submodule init
git submodule update
cd submodules/imgui && git checkout docking

cd ../..

# cd ../.. && sed -i -e 's/add_library(jpeg STATIC)/add_library(jpeg SHARED)/g' submodules/SDL_image/external/jpeg-9d/CMakeLists.txt
sed -i -e 's/if (ImGui::IsClippedEx(m_WidgetRect, id, false))/if (ImGui::IsClippedEx(m_WidgetRect, id))/g' submodules/imguiNodeEditor/imgui_canvas.cpp

FOLDER=$PWD

mkdir libs/SDL
cd submodules/SDL && ./autogen.sh && ./configure --prefix="$FOLDER"/libs/SDL && make && make install && cd ../..
mkdir libs/SDL_image
cd submodules/SDL_image && ./autogen.sh && ./configure --prefix="$FOLDER"/libs/SDL_image && make && make install && cd ../..
mkdir libs/SDL_mixer
cd submodules/SDL_mixer && ./autogen.sh && ./configure --prefix="$FOLDER"/libs/SDL_mixer && make && make install && cd ../..
mkdir libs/SDL_net
cd submodules/SDL_net && ./autogen.sh && ./configure --prefix="$FOLDER"/libs/SDL_net && make && make install && cd ../..
cd submodules/freetype && ./autogen.sh && ./configure && make && sudo make install && cd ../..

# for mac install automake, autoconf and other that will show error. Install xquartx, sdl2, sdl2_image, sdl2_net, sdl2_mixer. Execute this setup