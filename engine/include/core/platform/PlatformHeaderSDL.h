// Created by borja on 8/2/22.


#ifndef ENGINE2_0_PLATFORM_HEADER_SDL_H
#define ENGINE2_0_PLATFORM_HEADER_SDL_H

#ifdef __ANDROID__
    #include "../targets/engineAndroid/SDL/include/SDL.h"
#elif __linux__
    #include <SDL2/SDL.h>
#endif

#endif //ENGINE2_0_PLATFORM_HEADER_SDL_H
