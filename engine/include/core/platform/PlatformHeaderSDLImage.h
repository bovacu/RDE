// Created by borja on 9/2/22.


#ifndef GDE_PLATFORM_HEADER_SDL_IMAGE_H
#define GDE_PLATFORM_HEADER_SDL_IMAGE_H

#include "core/Core.h"

#if IS_MOBILE()
#include "SDL_image.h"
#else
#include "SDL2/SDL_image.h"
#endif

#endif //GDE_PLATFORM_HEADER_SDL_IMAGE_H
