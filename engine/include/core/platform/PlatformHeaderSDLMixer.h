// Created by borja on 8/2/22.


#ifndef ENGINE2_0_PLATFORM_HEADER_SDL_MIXER_H
#define ENGINE2_0_PLATFORM_HEADER_SDL_MIXER_H

#include "core/Core.h"

#if IS_MOBILE()
#include "SDL_mixer.h"
#else
#include "SDL2/SDL_mixer.h"
#endif

#endif //ENGINE2_0_PLATFORM_HEADER_SDL_MIXER_H
