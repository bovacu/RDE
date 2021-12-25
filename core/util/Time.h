#pragma once

class ITimer {
    public:
        virtual float getTimeSinceStart() = 0;
        virtual float restart() = 0;

        virtual ~ITimer() = default;
};

#include <SFML/System/Clock.hpp>
class SFMLTimer : public ITimer {
    private:
        sf::Clock clock;

    public:
        float getTimeSinceStart()   override;
        float restart()             override;

        ~SFMLTimer()                override = default;
};

