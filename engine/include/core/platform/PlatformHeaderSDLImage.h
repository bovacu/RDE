// Created by borja on 9/2/22.


#ifndef ENGINE2_0_PLATFORM_HEADER_SDL_IMAGE_H
#define ENGINE2_0_PLATFORM_HEADER_SDL_IMAGE_H

#include "core/Core.h"

#if IS_MOBILE()
    #include "../SDL_image/SDL_image.h"
#elif IS_DESKTOP()
    #include <SDL2/SDL_image.h>
#endif

#endif //ENGINE2_0_PLATFORM_HEADER_SDL_IMAGE_H
