#pragma once

#ifndef RDE_TIME_STEP_H
#define RDE_TIME_STEP_H

#include "core/Core.h"

namespace RDE {

    class Delta {
        private:
            float time;

        public:
			RDE_FUNC Delta(float _time = 0.0f) : time(_time) {  }

			RDE_FUNC operator float() const { return time; }

			RDE_FUNC_ND float getSeconds() const { return time; }
			RDE_FUNC_ND float getMilliseconds() const { return time * 1000.0f; }
    };

}

#endif //RDE_TIME_STEP_H
