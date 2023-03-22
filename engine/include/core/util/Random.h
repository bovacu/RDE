//
// Created by borja on 14/12/21.
//

#ifndef RDE_RANDOM_H
#define RDE_RANDOM_H

#include <random>
#include "core/Core.h"

namespace RDE {

    struct Probability {
        float prob = 0.0f;
        bool happened = false;
    };

    class Random {
        private:
            std::random_device rd;
            std::mt19937 mt;

        public:
			RDE_FUNC_EXPLICIT Random(long _seed = -1) : mt(rd()) {
                seedRandom(_seed);
            };

			RDE_FUNC void seedRandom(long _seed) {
                if(_seed == -1) mt.seed(rd());
                else mt.seed(_seed);
            }

			RDE_FUNC_ND int randomi(int _min, int _max) {
                if(_min > _max) {
                    int _aux = _min;
                    _min = _max;
                    _max = _aux;
                }
                std::uniform_int_distribution<int> _dist(_min, _max);
                return _dist(mt);
            }

			RDE_FUNC_ND float randomf(float _min, float _max) {
                if(_min > _max) {
                    float _aux = _min;
                    _min = _max;
                    _max = _aux;
                }
                std::uniform_real_distribution<float> _dist(_min, _max);
                return _dist(mt);
            }

			RDE_FUNC_ND Probability probability(float _chanceToHappen) {
                if(_chanceToHappen > 1.0f) _chanceToHappen = 1.0f;
                if(_chanceToHappen < 0.0f) _chanceToHappen = 0.0f;

                float _chance = 1.f - _chanceToHappen;
                float _leftProbability = randomf(0.0f, 1.0f);

                Probability _p {_leftProbability, _leftProbability >= _chance};
                return _p;
            }

			RDE_FUNC_ND Probability probability(int _chanceToHappen) {
                if(_chanceToHappen > 100)   _chanceToHappen = 100;
                if(_chanceToHappen < 0)     _chanceToHappen = 0;
                return probability((float)_chanceToHappen / 100.f);
            }
    };

}

#endif //RDE_RANDOM_H
