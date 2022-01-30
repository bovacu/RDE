#include "Clock.h"

#include <GLFW/glfw3.h>
#include "Delta.h"

namespace engine {

    Clock::Clock() : startTime((float)glfwGetTime()) {  }

    float Clock::getElapsedTimeSc() const {
        Delta _now = (float)glfwGetTime();
        return _now - startTime;
    }

    float Clock::getElapsedTimeMs() const {
        Delta _now = (float)glfwGetTime();
        return (_now - startTime) * 1000;
    }

    float Clock::restart() {
        Delta _now = (float)glfwGetTime();
        float _elapsed = _now - startTime;
        startTime = _now;
        return _elapsed;
    }
}