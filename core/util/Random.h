//
// Created by borja on 14/12/21.
//

#ifndef RESOURCES_GAME_RANDOM_H
#define RESOURCES_GAME_RANDOM_H

#include <random>

struct Probability {
    float prob = 0.0f;
    bool happened = false;
};

class Random {
    private:
    std::random_device rd;
    std::mt19937 mt;

    public:


    public:
    explicit Random() : mt(rd()) {  };

    int randomi(int _min, int _max) {
        if(_min > _max) {
            int _aux = _min;
            _min = _max;
            _max = _min;
        }
        std::uniform_int_distribution<int> _dist(_min, _max);
        return _dist(this->mt);
    }

    float randomf(float _min, float _max) {
        if(_min > _max) {
            float _aux = _min;
            _min = _max;
            _max = _min;
        }
        std::uniform_real_distribution<float> _dist(_min, _max);
        return _dist(this->mt);
    }

    Probability probability(float _chanceToHappen) {
        if(_chanceToHappen > 1.0f) _chanceToHappen = 1.0f;
        if(_chanceToHappen < 0.0f) _chanceToHappen = 0.0f;

        float _chance = 1.f - _chanceToHappen;
        float _leftProbability = this->randomf(0.0f, 1.0f);

        Probability _p {_leftProbability, _leftProbability >= _chance};
        return _p;
    }

    Probability probability(int _chanceToHappen) {
        if(_chanceToHappen > 100)   _chanceToHappen = 100;
        if(_chanceToHappen < 0)     _chanceToHappen = 0;
        return this->probability((float)_chanceToHappen / 100.f);
    }
};

#endif //RESOURCES_GAME_RANDOM_H
