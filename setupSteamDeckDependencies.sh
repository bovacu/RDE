#!/bin/bash

# First install brew as it installs everything in /home and steam deck has very limited space in /, /opt ...
if ! [ -x "$(command -v brew)" ]; then
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    test -d ~/.linuxbrew && eval "$(~/.linuxbrew/bin/brew shellenv)"
    test -d /home/linuxbrew/.linuxbrew && eval "$(/home/linuxbrew/.linuxbrew/bin/brew shellenv)"
    test -r ~/.bash_profile && echo "eval \"\$($(brew --prefix)/bin/brew shellenv)\"" >> ~/.bash_profile
    echo "eval \"\$($(brew --prefix)/bin/brew shellenv)\"" >> ~/.profile
fi

brew install cmake
brew install llvm
brew install ninja

sudo steamos-readonly disable
sudo pacman-key --populate archlinux
sudo pacman -Sy base-devel
sudo pacman -Syu glibc
sudo pacman -Sy gcc-libs
sudo pacman -Sy gcc
sudo pacman -Sy linux-api-headers
sudo pacman -Sy libarchive
