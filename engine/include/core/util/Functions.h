// Created by borja on 23/12/21.


#ifndef RESOURCES_GAME_FUNCTIONS_H
#define RESOURCES_GAME_FUNCTIONS_H

#include "core/Engine.h"
#include "core/render/Camera.h"

namespace GDE {

    class Util {

        public:
            template<typename T>
            static T clamp(T _value, T _minValue, T _maxValue) {
                return (_value < _minValue) ? _minValue : (_value > _maxValue) ? _maxValue : _value;
            }

            #define clampF(_value, _minValue, _maxValue) clamp<float>(_value, _minValue, _maxValue);
            #define clampD(_value, _minValue, _maxValue) clamp<double>(_value, _minValue, _maxValue);
            #define clampI(_value, _minValue, _maxValue) clamp<int>(_value, _minValue, _maxValue);
            #define clampL(_value, _minValue, _maxValue) clamp<long>(_value, _minValue, _maxValue);

            static std::string getFileNameFromPath(const std::string& _path)  {
                std::string base_filename = _path.substr(_path.find_last_of("/\\") + 1);
                std::string::size_type const p(base_filename.find_last_of('.'));
                return base_filename.substr(0, p);
            }

            static std::string getPathFromFilePath(const std::string& _path) {
                return _path.substr(0, _path.find_last_of("/\\") + 1);
            }

            static std::string getFileExtension(const std::string& _path) {
                std::string base_filename = _path.substr(_path.find_last_of("/\\") + 1);
                return base_filename.substr(base_filename.find_last_of('.') + 1, base_filename.size());
            }

            static Vec2F worldToScreenCoords(IViewPort* _viewport, const Vec2F& _position) {
                auto _windowSize = _viewport->getDeviceResolution();
                return {_position.x * _viewport->getAspectRatio() * _viewport->getScalingFactor().x / ((float)_windowSize.x / 2),
                        _position.y * _viewport->getScalingFactor().y / ((float)_windowSize.y / 2)};
            }

            #if !defined(IOS_PLATFORM)
            [[gnu::warning("Probably broken some versions ago, check it out again")]]
            #endif
            static Vec2F screenToWorldCoords(IViewPort* _viewport, const Vec2F& _position) {
                auto _windowSize = _viewport->getDeviceResolution();
                return {_position.x / _viewport->getAspectRatio() * ((float)_windowSize.x / 2), _position.y * ((float)_windowSize.y / 2)};
            }

            static Vec2F worldToScreenSize(IViewPort* _viewport, const Vec2F& _size) {
                auto _windowSize = _viewport->getDeviceResolution();
                return {_size.x * _viewport->getAspectRatio() * _viewport->getScalingFactor().x / ((float)_windowSize.x),
                        _size.y * _viewport->getScalingFactor().y / ((float)_windowSize.y)};
            }

            static void worldToScreenSize(IViewPort* _viewport, float& _x, float& _y) {
                auto _new = worldToScreenCoords(_viewport, {_x, _y});
                _x = _new.x;
                _y = _new.y;
            }

            static void screenToWorldCoords(IViewPort* _viewport, float& _x, float& _y) {
                auto _new = screenToWorldCoords(_viewport, {_x, _y});
                _x = _new.x;
                _y = _new.y;
            }
    };


}

#endif //RESOURCES_GAME_FUNCTIONS_H
