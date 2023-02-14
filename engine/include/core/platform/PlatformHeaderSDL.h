// Created by borja on 8/2/22.


#ifndef RDE_PLATFORM_HEADER_SDL_H
#define RDE_PLATFORM_HEADER_SDL_H

#include "core/Core.h"
#if IS_ANDROID()
    #ifndef SDL_DISABLE_MMINTRIN_H
        #define SDL_DISABLE_MMINTRIN_H 
    #endif

    #ifndef SDL_DISABLE_XMMINTRIN_H
        #define SDL_DISABLE_XMMINTRIN_H 
    #endif
    
    #ifndef SDL_DISABLE_EMMINTRIN_H
        #define SDL_DISABLE_EMMINTRIN_H 
    #endif

    #ifndef SDL_DISABLE_PMMINTRIN_H
        #define SDL_DISABLE_PMMINTRIN_H 
    #endif

    #ifndef SDL_DISABLE_IMMINTRIN_H
        #define SDL_DISABLE_IMMINTRIN_H
    #endif 
#endif
#include "SDL.h"

#endif //RDE_PLATFORM_HEADER_SDL_H
