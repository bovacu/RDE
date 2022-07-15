#include "core/util/Clock.h"
#include "core/util/Delta.h"
#include "SDL_timer.h"
#include "core/Core.h"

namespace GDE {

    #if IS_IOS()
    Clock::Clock() : startTime(SDL_GetTicks()) {  }
    #else
    Clock::Clock() : startTime(SDL_GetTicks64()) {  }
    #endif

    uint64_t Clock::getElapsedTimeSc() const {
        #if IS_IOS()
        uint64_t _now = SDL_GetTicks();
        #else
        uint64_t _now = SDL_GetTicks();
        #endif
       
        return (_now - startTime) / 1000;
    }

    uint64_t Clock::getElapsedTimeMs() const {
        #if IS_IOS()
        uint64_t _now = SDL_GetTicks();
        #else
        uint64_t _now = SDL_GetTicks();
        #endif

        return _now - startTime;
    }

    uint64_t Clock::restart() {
        #if IS_IOS()
        uint64_t _now = SDL_GetTicks();
        #else
        uint64_t _now = SDL_GetTicks();
        #endif
        
        auto _elapsed = _now - startTime;
        startTime = _now;
        return _elapsed;
    }
}
