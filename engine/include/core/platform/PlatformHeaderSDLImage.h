// Created by borja on 9/2/22.


#ifndef RDE_PLATFORM_HEADER_SDL_IMAGE_H
#define RDE_PLATFORM_HEADER_SDL_IMAGE_H

#include "core/Core.h"

#if IS_MOBILE()
#include "SDL_image.h"
#else
#include "SDL2/SDL_image.h"
#endif

#endif //RDE_PLATFORM_HEADER_SDL_IMAGE_H
