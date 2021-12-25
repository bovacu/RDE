// Created by borja on 23/12/21.


#ifndef RESOURCES_GAME_FUNCTIONS_H
#define RESOURCES_GAME_FUNCTIONS_H

template<typename T>
T clamp(T _value, T _minValue, T _maxValue) {
    return (_value < _minValue) ? _minValue : (_value > _maxValue) ? _maxValue : _value;
}

#define clampF(_value, _minValue, _maxValue) clamp<float>(_value, _minValue, _maxValue);
#define clampD(_value, _minValue, _maxValue) clamp<double>(_value, _minValue, _maxValue);
#define clampI(_value, _minValue, _maxValue) clamp<int>(_value, _minValue, _maxValue);
#define clampL(_value, _minValue, _maxValue) clamp<long>(_value, _minValue, _maxValue);

#endif //RESOURCES_GAME_FUNCTIONS_H
