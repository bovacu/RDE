GDE_PATH="$1"
cd "$GDE_PATH"/GDE

FOLDER="$PWD"
export SUDO_ASKPASS=/usr/bin/ssh-askpass
cd submodules/freetype && ./autogen.sh && ./configure && make && sudo --askpass make install && cd ../..