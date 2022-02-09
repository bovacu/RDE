#pragma once

#include "../../SDL/include/SDL_stdinc.h"
#include "../../SDL/include/SDL.h"
#define XBRZLIB_RELATIVEPATHSDL
#include "../../SDL_mixer/include/SDL_mixer.h"
#include "../../freetype/include/ft2build.h"
#include <utility>

namespace engine::sdl
{
    std::pair<uint32_t, uint32_t> getDisplaySize();

    SDL_Window* createWindow(const uint32_t& windowFlags);
} // namespace ast::sdl
