#pragma once

#ifndef TIMESTEP_H
#define TIMESTEP_H

namespace GDE {

    class Delta {
        private:
            float time;

        public:
            Delta(float _time = 0.0f) : time(_time) {  }

            operator float() const { return time; }

            float GetSeconds() const { return time; }
            float GetMilliseconds() const { return time * 1000.0f; }
    };

}

#endif //TIMESTEP_H
