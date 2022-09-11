#pragma once

#ifndef GDE_TIME_STEP_H
#define GDE_TIME_STEP_H

namespace GDE {

    class Delta {
        private:
            float time;

        public:
            Delta(float _time = 0.0f) : time(_time) {  }

            operator float() const { return time; }

            float getSeconds() const { return time; }
            float getMilliseconds() const { return time * 1000.0f; }
    };

}

#endif //GDE_TIME_STEP_H
