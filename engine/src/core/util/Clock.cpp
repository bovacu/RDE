#include "core/util/Clock.h"
#include "core/util/Delta.h"
#include "SDL_timer.h"

namespace GDE {

    Clock::Clock() : startTime(SDL_GetTicks64()) {  }

    uint64_t Clock::getElapsedTimeSc() const {
        uint64_t _now = SDL_GetTicks64();
        return (_now - startTime) / 1000;
    }

    uint64_t Clock::getElapsedTimeMs() const {
        uint64_t _now = SDL_GetTicks64();
        return _now - startTime;
    }

    uint64_t Clock::restart() {
        uint64_t _now = SDL_GetTicks64();
        auto _elapsed = _now - startTime;
        startTime = _now;
        return _elapsed;
    }
}