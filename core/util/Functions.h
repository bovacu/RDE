// Created by borja on 23/12/21.


#ifndef RESOURCES_GAME_FUNCTIONS_H
#define RESOURCES_GAME_FUNCTIONS_H

namespace engine::util {
    template<typename T>
    T clamp(T _value, T _minValue, T _maxValue) {
        return (_value < _minValue) ? _minValue : (_value > _maxValue) ? _maxValue : _value;
    }

    #define clampF(_value, _minValue, _maxValue) clamp<float>(_value, _minValue, _maxValue);
    #define clampD(_value, _minValue, _maxValue) clamp<double>(_value, _minValue, _maxValue);
    #define clampI(_value, _minValue, _maxValue) clamp<int>(_value, _minValue, _maxValue);
    #define clampL(_value, _minValue, _maxValue) clamp<long>(_value, _minValue, _maxValue);

    inline std::string getFileNameFromPath(const std::string& _path) {
        std::string base_filename = _path.substr(_path.find_last_of("/\\") + 1);
        std::string::size_type const p(base_filename.find_last_of('.'));
        return base_filename.substr(0, p);
    }
}

#endif //RESOURCES_GAME_FUNCTIONS_H
