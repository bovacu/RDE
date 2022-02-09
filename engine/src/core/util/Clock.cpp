#include "core/util/Clock.h"

#include "core/util/Delta.h"

namespace engine {

    Clock::Clock() : startTime(0) {  }

    float Clock::getElapsedTimeSc() const {
//        Delta _now = (float)glfwGetTime();
//        return _now - startTime;
        return 0;
    }

    float Clock::getElapsedTimeMs() const {
//        Delta _now = (float)glfwGetTime();
//        return (_now - startTime) * 1000;
        return 0;
    }

    float Clock::restart() {
//        Delta _now = (float)glfwGetTime();
//        float _elapsed = _now - startTime;
//        startTime = _now;
//        return _elapsed;
        return 0;
    }
}