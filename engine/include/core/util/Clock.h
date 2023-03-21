#pragma once

#ifndef RDE_CLOCK_H
#define RDE_CLOCK_H

#include <cstdint>
#include "core/Core.h"

namespace RDE {

    /// This is a class that tracks the time passed since a specific moment.
    class Clock {
        private:
            uint64_t startTime;

        public:
			RDE_FUNC_EXPLICIT Clock();

            /// Returns the elapsed time since the clock started in seconds.
            /// @return the elapsed time in seconds.
			RDE_FUNC_ND uint64_t getElapsedTimeSc() const;

            /// Returns the elapsed time since the clock started in milliseconds.
            /// @return the elapsed time in milliseconds.
			RDE_FUNC_ND uint64_t getElapsedTimeMs() const;

            /// Restarts the clock and returns the elapsed time since the initialization of the clock.
            /// @return elapsed time since initialization in milliseconds.
			RDE_FUNC uint64_t restart();
    };

}

#endif //RDE_CLOCK_H
