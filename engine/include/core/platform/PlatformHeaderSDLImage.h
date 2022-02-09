// Created by borja on 9/2/22.


#ifndef ENGINE2_0_PLATFORM_HEADER_SDL_IMAGE_H
#define ENGINE2_0_PLATFORM_HEADER_SDL_IMAGE_H

#ifdef __ANDROID__
    #include "../../targets/engineAndroid/SDL_image/SDL_image.h"
#elif __linux__
    #include <SDL2/SDL_image.h>
#endif

#endif //ENGINE2_0_PLATFORM_HEADER_SDL_IMAGE_H
