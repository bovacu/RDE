#pragma once

#ifndef CLOCK_H
#define CLOCK_H

#include <cstdint>

namespace engine {

    /// This is a class that tracks the time passed since a specific moment.
    class Clock {
        private:
            uint64_t startTime;

        public:
            explicit Clock();

            /// Returns the elapsed time since the clock started in seconds.
            /// @return the elapsed time in seconds.
            [[nodiscard]] uint64_t getElapsedTimeSc() const;

            /// Returns the elapsed time since the clock started in milliseconds.
            /// @return the elapsed time in milliseconds.
            [[nodiscard]] uint64_t getElapsedTimeMs() const;

            /// Restarts the clock and returns the elapsed time since the initialization of the clock.
            /// @return elapsed time since initialization in milliseconds.
            uint64_t restart();
    };

}

#endif //CLOCK_H
