//
// Created by borja on 14/12/21.
//

#ifndef RESOURCES_GAME_EASING_H
#define RESOURCES_GAME_EASING_H

#include <cmath>

class Easing {
    public:
    /// Linear
    static float inLinear(float _currentTime, float _startValue, float _changeInValue, float _duration) {
        return _changeInValue * _currentTime / _duration + _startValue;
    }

    /// Quadratic
    static float inQuadratic(float _currentTime, float _startValue, float _changeInValue, float _duration) {
        _currentTime /= _duration;
        return _changeInValue * _currentTime * _currentTime + _startValue;
    }

    static float outQuadratic(float _currentTime, float _startValue, float _changeInValue, float _duration) {
        _currentTime /= _duration;
        return -_changeInValue * _currentTime * (_currentTime - 2) + _startValue;
    }

    static float inOutQuadratic(float _currentTime, float _startValue, float _changeInValue, float _duration) {
        _currentTime /= _duration / 2;
        if (_currentTime < 1) return _changeInValue / 2 * _currentTime * _currentTime + _startValue;
        _currentTime--;
        return -_changeInValue / 2 * (_currentTime * (_currentTime - 2) - 1) + _startValue;
    }

    /// Cubic
    static float inCubic(float _currentTime, float _startValue, float _changeInValue, float _duration) {
        _currentTime /= _duration;
        return _changeInValue * _currentTime * _currentTime * _currentTime + _startValue;
    }

    static float outCubic(float _currentTime, float _startValue, float _changeInValue, float _duration) {
        _currentTime /= _duration;
        _currentTime--;
        return _changeInValue * (_currentTime * _currentTime * _currentTime + 1) + _startValue;
    }

    static float inOutCubic(float _currentTime, float _startValue, float _changeInValue, float _duration) {
        _currentTime /= _duration / 2;
        if (_currentTime < 1) return _changeInValue / 2 * _currentTime * _currentTime * _currentTime + _startValue;
        _currentTime -= 2;
        return _changeInValue / 2 * (_currentTime * _currentTime * _currentTime + 2) + _startValue;
    }

    /// Quartic
    static float inQuartic(float _currentTime, float _startValue, float _changeInValue, float _duration) {
        _currentTime /= _duration;
        return _changeInValue * _currentTime * _currentTime * _currentTime * _currentTime + _startValue;
    }

    static float outQuartic(float _currentTime, float _startValue, float _changeInValue, float _duration) {
        _currentTime /= _duration;
        _currentTime--;
        return -_changeInValue * (_currentTime * _currentTime * _currentTime * _currentTime - 1) + _startValue;
    }

    static float inOutQuartic(float _currentTime, float _startValue, float _changeInValue, float _duration) {
        _currentTime /= _duration / 2;
        if (_currentTime < 1)
            return _changeInValue / 2 * _currentTime * _currentTime * _currentTime * _currentTime + _startValue;
        _currentTime -= 2;
        return -_changeInValue / 2 * (_currentTime * _currentTime * _currentTime * _currentTime - 2) + -_startValue;
    }

    /// Quintic
    static float inQuintic(float _currentTime, float _startValue, float _changeInValue, float _duration) {
        _currentTime /= _duration;
        return _changeInValue * _currentTime * _currentTime * _currentTime * _currentTime * _currentTime + _startValue;
    }

    static float outQuintic(float _currentTime, float _startValue, float _changeInValue, float _duration) {
        _currentTime /= _duration;
        _currentTime--;
        return _changeInValue * (_currentTime * _currentTime * _currentTime * _currentTime * _currentTime + 1) +
               _startValue;
    }

    static float inOutQuintic(float _currentTime, float _startValue, float _changeInValue, float _duration) {
        _currentTime /= _duration / 2;
        if (_currentTime < 1)
            return _changeInValue / 2 * _currentTime * _currentTime * _currentTime * _currentTime * _currentTime +
                   _startValue;
        _currentTime -= 2;
        return _changeInValue / 2 * (_currentTime * _currentTime * _currentTime * _currentTime * _currentTime + 2) +
               _startValue;
    }

    /// Sinusoidal
    static float inSine(float _currentTime, float _startValue, float _changeInValue, float _duration) {
        return -_changeInValue * (float) std::cos(_currentTime / _duration * (M_PI / 2)) + _changeInValue + _startValue;
    }

    static float outSine(float _currentTime, float _startValue, float _changeInValue, float _duration) {
        return _changeInValue * (float) std::sin(_currentTime / _duration * (M_PI / 2)) + _startValue;
    }

    static float inOutSine(float _currentTime, float _startValue, float _changeInValue, float _duration) {
        return -_changeInValue / 2 * ((float) std::cos(M_PI * _currentTime / _duration) - 1) + _startValue;
    }

    /// Exponential
    static float inExponential(float _currentTime, float _startValue, float _changeInValue, float _duration) {
        return _changeInValue * (float) std::pow(2, 10 * (_currentTime / _duration - 1)) + _startValue;
    }

    static float outExponential(float _currentTime, float _startValue, float _changeInValue, float _duration) {
        return _changeInValue * (-(float) std::pow(2, -10 * _currentTime / _duration) + 1) + _startValue;
    }

    static float inOutExponential(float _currentTime, float _startValue, float _changeInValue, float _duration) {
        _currentTime /= _duration / 2;
        if (_currentTime < 1) return _changeInValue / 2 * (float) std::pow(2, 10 * (_currentTime - 1)) + _startValue;
        _currentTime--;
        return _changeInValue / 2 * (-(float) std::pow(2, -10 * _currentTime) + 2) + _startValue;
    }

    /// Exponential
    static float inCircular(float _currentTime, float _startValue, float _changeInValue, float _duration) {
        _currentTime /= _duration;
        return -_currentTime * ((float) std::sqrt(1 - _currentTime * _currentTime) - 1) + _startValue;
    }

    static float outCircular(float _currentTime, float _startValue, float _changeInValue, float _duration) {
        _currentTime /= _duration;
        _currentTime--;
        return _changeInValue * (float) std::sqrt(1 - _currentTime * _currentTime) + _startValue;
    }

    static float inOutCircular(float _currentTime, float _startValue, float _changeInValue, float _duration) {
        _currentTime /= _duration / 2;
        if (_currentTime < 1)
            return -_changeInValue / 2 * ((float) std::sqrt(1 - _currentTime * _currentTime) - 1) + _startValue;
        _currentTime -= 2;
        return _changeInValue / 2 * ((float) std::sqrt(1 - _currentTime * _currentTime) + 1) + _startValue;
    }
};

#endif //RESOURCES_GAME_EASING_H
