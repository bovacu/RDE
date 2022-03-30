// Created by borja on 23/12/21.


#ifndef RESOURCES_GAME_FUNCTIONS_H
#define RESOURCES_GAME_FUNCTIONS_H

#include "core/render/window/Window.h"
#include "core/Engine.h"

namespace engine {

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

            static Vec2F worldToScreenCoords(const Vec2F& _position, Window* _window, float _aspectRatio) {
                return {_position.x * _aspectRatio / ((float)_window->getWindowSize().x / 2), _position.y / ((float)_window->getWindowSize().y / 2)};
            }

            static Vec2F screenToWorldCoords(const Vec2F& _position, Window* _window, float _aspectRatio) {
                return {_position.x / _aspectRatio * ((float)_window->getWindowSize().x / 2), _position.y * ((float)_window->getWindowSize().y / 2)};
            }

            static Vec2F worldToScreenSize(const Vec2F& _size, Window* _window, float _aspectRatio) {
                return {_size.x * _aspectRatio / ((float)_window->getWindowSize().x), _size.y / ((float)_window->getWindowSize().y)};
            }

            static void worldToScreenSize(float& _x, float& _y) {
                auto _window = Engine::get().getWindowSize();
                auto _aspectRatio = Engine::get().getScene()->getMainCamera()->getViewport()->getAspectRatio();
                _x = _x * _aspectRatio / ((float)_window.x / 2);
                _y = _y / ((float)_window.y / 2);
            }

            static void screenToWorldCoords(float& _x, float& _y) {
                auto _window = Engine::get().getWindowSize();
                auto _aspectRatio = Engine::get().getScene()->getMainCamera()->getViewport()->getAspectRatio();
                _x = _x / _aspectRatio * ((float)_window.x / 2);
                _y = _y * ((float)_window.y / 2);
            }
    };


}

#endif //RESOURCES_GAME_FUNCTIONS_H
