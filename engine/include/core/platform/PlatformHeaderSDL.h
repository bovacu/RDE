// Created by borja on 8/2/22.


#ifndef ENGINE2_0_PLATFORM_HEADER_SDL_H
#define ENGINE2_0_PLATFORM_HEADER_SDL_H

#include "core/Core.h"

#if IS_MOBILE()
    #include "SDL/include/SDL.h"
#elif IS_DESKTOP()
    #include <SDL2/SDL.h>
#endif

#endif //ENGINE2_0_PLATFORM_HEADER_SDL_H
