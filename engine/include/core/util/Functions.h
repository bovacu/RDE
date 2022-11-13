// Created by borja on 23/12/21.


#ifndef GDE_FUNCTIONS_H
#define GDE_FUNCTIONS_H

#include "core/render/elements/ViewPort.h"

namespace GDE {

    class Util {

        public:
            template<typename T>
            static T clamp(T _value, T _minValue, T _maxValue) {
                return (_value < _minValue) ? _minValue : (_value > _maxValue) ? _maxValue : _value;
            }

            #define clampF(_value, _minValue, _maxValue) Util::clamp<float>(_value, _minValue, _maxValue)
            #define clampD(_value, _minValue, _maxValue) Util::clamp<double>(_value, _minValue, _maxValue)
            #define clampI(_value, _minValue, _maxValue) Util::clamp<int>(_value, _minValue, _maxValue)
            #define clampL(_value, _minValue, _maxValue) Util::clamp<long>(_value, _minValue, _maxValue)

            static std::string getFileNameFromPath(const std::string& _path) {
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

            static Vec2F worldToScreenCoords(const ViewPort& _viewport, const Vec2F& _position) {
                auto _windowSize = _viewport.getVirtualResolution();
                return {_position.x * _viewport.getVirtualAspectRatio() / ((float)_windowSize.x * 0.5f), _position.y / ((float)_windowSize.y * 0.5f) };
            }

            #if !IS_MAC() && !IS_WINDOWS() && !IS_MOBILE()
            [[gnu::warning("Probably broken some versions ago, check it out again")]]
            #endif
            static Vec2F screenToWorldCoords(const ViewPort& _viewport, const Vec2F& _position) {
                auto _windowSize = _viewport.getVirtualResolution();
                return {_position.x / _viewport.getVirtualAspectRatio() * ((float)_windowSize.x * 0.5f),
                        _position.y * ((float)_windowSize.y * 0.5f) };
            }

            static Vec2F worldToScreenSize(const ViewPort& _viewport, const Vec2F& _size) {
                auto _virtualRes = _viewport.getVirtualResolution();
                return {_size.x * _viewport.getVirtualAspectRatio() / ((float)_virtualRes.x),
                        _size.y / ((float)_virtualRes.y) };
            }

            static Vec2F worldToScreenCoordsUI(const ViewPort& _viewport, const Vec2F& _position) {
                auto _physicalRes = _viewport.getDeviceResolution();
                return {_position.x * _viewport.getPhysicalAspectRatio() / ((float)_physicalRes.x * 0.5f), _position.y / ((float)_physicalRes.y * 0.5f) };
            }

            static Vec2F worldToScreenSizeUI(const ViewPort& _viewport, const Vec2F& _size) {
                auto _physicalRes = _viewport.getDeviceResolution();
                return {_size.x * _viewport.getPhysicalAspectRatio() / ((float)_physicalRes.x), _size.y / ((float)_physicalRes.y) };
            }

            static void worldToScreenSize(const ViewPort& _viewport, float& _x, float& _y) {
                auto _new = worldToScreenCoords(_viewport, {_x, _y});
                _x = _new.x;
                _y = _new.y;
            }

            static void screenToWorldCoords(const ViewPort& _viewport, float& _x, float& _y) {
                auto _new = screenToWorldCoords(_viewport, {_x, _y});
                _x = _new.x;
                _y = _new.y;
            }
    };


}

#endif //GDE_FUNCTIONS_H
