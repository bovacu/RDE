#pragma once

#ifndef CLOCK_H
#define CLOCK_H

namespace engine {

    /// This is a class that tracks the time passed since a specific moment.
    class Clock {
        private:
            float startTime;

        public:
            explicit Clock();

            /// Returns the elapsed time since the clock started in milliseconds.
            /// @return the elapsed time in milliseconds.
            [[nodiscard]] float getElapsedTimeSc() const;

            /// Returns the elapsed time since the clock started in nanoseconds.
            /// @return the elapsed time in nanoseconds.
            [[nodiscard]] float getElapsedTimeMs() const;

            /// Restarts the clock and returns the elapsed time since the initialization of the clock.
            /// @return elapsed time since initialization in milliseconds.
            float restart();
    };

}

#endif //CLOCK_H
