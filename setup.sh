#!/bin/bash

if [[ "$OSTYPE" == "linux-gnu"* ]]
then
    echo "Linux (for now only supported linux debian based systems)"
    sudo apt-get install build-essential cmake clang git autoconf libtool m4 automake xorg-dev mesa-common-dev libgl1-mesa-dev libglu1-mesa-dev mesa-utils libsamplerate-dev libudev-dev libdbus-1-dev libibus-1.0-dev fcitx-libs-dev zlib1g bzip2 libpng-dev libdbus-1-dev libegl-dev libopengl-dev

    IS_WAYLAND=$(echo $XDG_SESSION_TYPE)
    if [[ "$IS_WAYLAND" != "x11" ]]; then
        sudo apt-get install meson libwayland-dev wayland-protocols libpango1.0-dev libxkbcommon-dev
    fi
elif [[ "$OSTYPE" == "darwin"* ]]
then
    echo "MacOs"
    brew install sdl2 sdl2_image sdl2_mixer sdl2_net freetype freeglut
else
    echo "Other system, specify which is your system in a GitHub issue so it's support is included in next releases"
fi


git submodule init
git submodule update
cd submodules/imgui && git checkout docking
cd ../..
cd submodules/imguiNodeEditor && git checkout develop
cd ../..

cd submodules/libdecor && git checkout -b 'christian-rauch/libdecor-gtk_cairo_single' && meson build --buildtype release && meson install -C build
cd ../..

# cd ../.. && sed -i -e 's/add_library(jpeg STATIC)/add_library(jpeg SHARED)/g' submodules/SDL_image/external/jpeg-9d/CMakeLists.txt

FOLDER=$PWD

mkdir libs/SDL
cd submodules/SDL && ./autogen.sh && ./configure --prefix="$FOLDER"/libs/SDL && make && make && cd ../..
mkdir libs/SDL_image
cd submodules/SDL_image && ./autogen.sh && ./configure --prefix="$FOLDER"/libs/SDL_image && make && make && cd ../..
mkdir libs/SDL_mixer
cd submodules/SDL_mixer && ./autogen.sh && ./configure --prefix="$FOLDER"/libs/SDL_mixer && make && make && cd ../..
mkdir libs/SDL_net
cd submodules/SDL_net && ./autogen.sh && ./configure --prefix="$FOLDER"/libs/SDL_net && make && make && cd ../..
cd submodules/freetype && ./autogen.sh && ./configure && make && sudo make install && cd ../..
cd submodules/box2d && cmake . && sudo make && cd ../..
cd submodules/yaml-cpp && cmake . && sudo make && cd ../..