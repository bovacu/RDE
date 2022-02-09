// Created by borja on 8/2/22.


#ifndef ENGINE2_0_PLATFORM_HEADER_SDL_MIXER_H
#define ENGINE2_0_PLATFORM_HEADER_SDL_MIXER_H

#ifdef __ANDROID__
    #include "../../targets/engineAndroid/SDL_mixer/include/SDL_mixer.h"
#elif __linux__
    #include <SDL2/SDL_mixer.h>
#endif

#endif //ENGINE2_0_PLATFORM_HEADER_SDL_MIXER_H
