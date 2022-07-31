#!/bin/bash

if [[ "$OSTYPE" == "linux-gnu"* ]]
then
    echo "Linux (for now only supported linux debian based systems)"

    # Those are needed to compile the project and also to compile SDL2 properly statically linked
    sudo apt-get install build-essential cmake clang git autoconf libtool m4 automake xorg-dev mesa-common-dev libgl1-mesa-dev libglu1-mesa-dev mesa-utils libsamplerate-dev libudev-dev libdbus-1-dev libibus-1.0-dev fcitx-libs-dev zlib1g bzip2 libdbus-1-dev libegl-dev libopengl-dev

    # Those are needed to make SDL_image to work properly statically linked
    sudo apt-get install libpng-dev libjpeg-dev libwebp-dev libtiff-dev

    # Those are needed to make SDL_mixer to work properly statically linked
    sudo apt-get install libflac-dev libmodplug-dev libogg-dev libvorbis-dev libvorbisidec-dev libmpg123-dev libopus-dev libopusfile-dev

    IS_WAYLAND=$(echo $XDG_SESSION_TYPE)
    if [[ "$IS_WAYLAND" != "x11" ]]; then
        sudo apt-get install meson libwayland-dev wayland-protocols libpango1.0-dev libxkbcommon-dev
    fi
elif [[ "$OSTYPE" == "darwin"* ]]
then
    echo "MacOs"
    brew install freetype freeglut
else
    echo "Other system, specify which is your system in a GitHub issue so it's support is included in next releases"
fi


git submodule init
git submodule update
cd libs/imgui && git checkout docking
cd ../..

IS_WAYLAND=$(echo $XDG_SESSION_TYPE)
if [[ "$IS_WAYLAND" != "x11" ]]; then
    cd libs/libdecor && git checkout -b 'christian-rauch/libdecor-gtk_cairo_single' && meson build --buildtype release && meson install -C build
    cd ../..
fi

# cd ../.. && sed -i -e 's/add_library(jpeg STATIC)/add_library(jpeg SHARED)/g' submodules/SDL_image/external/jpeg-9d/CMakeLists.txt

FOLDER=$PWD
mkdir libs/SDL
cd submodules/SDL && ./autogen.sh && ./configure --prefix="$FOLDER"/libs/SDL && make && make install && cd ../..
mkdir libs/SDL_image
cd submodules/SDL_image && ./autogen.sh && ./configure --prefix="$FOLDER"/libs/SDL_image && make && make install && cd ../..
mkdir libs/SDL_mixer
cd submodules/SDL_mixer && ./autogen.sh && ./configure --prefix="$FOLDER"/libs/SDL_mixer && make && make install && cd ../..
mkdir libs/SDL_net
cd submodules/SDL_net && ./autogen.sh && ./configure --prefix="$FOLDER"/libs/SDL_net && make && make install && cd ../..
mkdir libs/freetype
mkdir libs/freetype/build
cd libs/freetype && ./autogen.sh && ./configure --without-zlib --without-png --without-bzip2 --without-harfbuzz --without-brotli --prefix="$FOLDER"/libs/freetype/build && make && make install && cd ../..