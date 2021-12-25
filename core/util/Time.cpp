#include "Time.h"

float SFMLTimer::getTimeSinceStart() {
    return clock.getElapsedTime().asSeconds();
}

float SFMLTimer::restart() {
    return clock.restart().asSeconds();
}
