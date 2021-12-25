#include "Input.h"
#include "core/Engine.h"
#include <GLFW/glfw3.h>

namespace engine {
    std::unordered_map<KeyCode, bool> Input::pressedKeyboardKeys = {
            {KeyCode::LeftControl,  false},
            {KeyCode::LeftSuper,    false},
            {KeyCode::LeftAlt, false},
            {KeyCode::Space,        false},
            {KeyCode::RightAlt,     false},
            {KeyCode::RightControl, false},
            {KeyCode::LeftShift,    false},
            {KeyCode::Z,            false},
            {KeyCode::X,            false},
            {KeyCode::C,            false},
            {KeyCode::V,            false},
            {KeyCode::B,            false},
            {KeyCode::N,            false},
            {KeyCode::M,            false},
            {KeyCode::Comma,        false},
            {KeyCode::Period,       false},
            {KeyCode::Slash,        false},
            {KeyCode::RightShift,   false},
            {KeyCode::CapsLock,     false},
            {KeyCode::A,            false},
            {KeyCode::S,            false},
            {KeyCode::D,            false},
            {KeyCode::F,            false},
            {KeyCode::G,            false},
            {KeyCode::H,            false},
            {KeyCode::J,            false},
            {KeyCode::K,            false},
            {KeyCode::L,            false},
            {KeyCode::Semicolon,    false},
            {KeyCode::Apostrophe,   false},
            {KeyCode::Enter,        false},
            {KeyCode::Tab,          false},
            {KeyCode::Q,            false},
            {KeyCode::W,            false},
            {KeyCode::E,            false},
            {KeyCode::R,            false},
            {KeyCode::T,            false},
            {KeyCode::Y,            false},
            {KeyCode::U,            false},
            {KeyCode::I,            false},
            {KeyCode::O,            false},
            {KeyCode::P,            false},
            {KeyCode::LeftBracket,  false},
            {KeyCode::RightBracket, false},
            {KeyCode::RightSuper,   false},
            {KeyCode::Backslash,    false},
            {KeyCode::GraveAccent,  false},
            {KeyCode::D0,           false},
            {KeyCode::D1,           false},
            {KeyCode::D2,           false},
            {KeyCode::D3,           false},
            {KeyCode::D4,           false},
            {KeyCode::D5,           false},
            {KeyCode::D6,           false},
            {KeyCode::D7,           false},
            {KeyCode::D8,           false},
            {KeyCode::D9,           false},
            {KeyCode::Minus,        false},
            {KeyCode::Equal,        false},
            {KeyCode::Backspace,    false},
            {KeyCode::Escape,       false},
            {KeyCode::F1,           false},
            {KeyCode::F2,           false},
            {KeyCode::F3,           false},
            {KeyCode::F4,           false},
            {KeyCode::F5,           false},
            {KeyCode::F6,           false},
            {KeyCode::F7,           false},
            {KeyCode::F8,           false},
            {KeyCode::F9,           false},
            {KeyCode::F10,          false},
            {KeyCode::F11,          false},
            {KeyCode::F12,          false},
            {KeyCode::PrintScreen,  false},
            {KeyCode::ScrollLock,   false},
            {KeyCode::Insert,       false},
            {KeyCode::Delete,       false},
            {KeyCode::Home,         false},
            {KeyCode::End,          false},
            {KeyCode::PageUp,       false},
            {KeyCode::PageDown,     false},
            {KeyCode::Right,        false},
            {KeyCode::Left,         false},
            {KeyCode::Up,           false},
            {KeyCode::Down,         false},

    };

    std::unordered_map<MouseCode, bool> Input::pressedMouseButtons = {
            {MouseCode::Button0,        false},
            {MouseCode::Button1,        false},
            {MouseCode::Button2,        false},
            {MouseCode::Button3,        false},
            {MouseCode::Button4,        false},
            {MouseCode::Button5,        false},
            {MouseCode::Button6,        false},
            {MouseCode::Button7,        false},
            {MouseCode::ButtonLast,     false},
            {MouseCode::ButtonLeft,     false},
            {MouseCode::ButtonMiddle,   false},
            {MouseCode::ButtonRight,    false},
    };

    bool Input::isKeyJustPressed(KeyCode _keyCode) {
        if(!Input::pressedKeyboardKeys[_keyCode]) {
            auto _window = static_cast<GLFWwindow*>(Engine::get().getWindow().getNativeWindow());
            auto _state = glfwGetKey(_window, static_cast<int32_t>(_keyCode));
            bool _result = _state == GLFW_PRESS;

            if(_result)
                Input::pressedKeyboardKeys[_keyCode] = true;

            return _result;
        }

        return false;
    }

    bool Input::isKeyPressed(KeyCode _key) {
        auto _window = static_cast<GLFWwindow*>(Engine::get().getWindow().getNativeWindow());
        auto _state = glfwGetKey(_window, static_cast<int32_t>(_key));
        return _state == GLFW_PRESS;
    }

    bool Input::isKeyReleased(KeyCode _key) {
        auto _window = static_cast<GLFWwindow*>(Engine::get().getWindow().getNativeWindow());
        auto _state = glfwGetKey(_window, static_cast<int32_t>(_key));
        return _state == GLFW_RELEASE;
    }

    bool Input::isMouseJustPressed(MouseCode _mouseButton) {
        if(!Input::pressedMouseButtons[_mouseButton]) {
            auto _window = static_cast<GLFWwindow*>(Engine::get().getWindow().getNativeWindow());
            auto _state = glfwGetMouseButton(_window, static_cast<int32_t>(_mouseButton));
            bool _result = _state == GLFW_PRESS;

            if(_result)
                Input::pressedMouseButtons[_mouseButton] = true;

            return _result;
        }

        return false;
    }

    bool Input::isMousePressed(MouseCode _button) {
        auto _window = static_cast<GLFWwindow*>(Engine::get().getWindow().getNativeWindow());
        auto _state = glfwGetMouseButton(_window, static_cast<int32_t>(_button));
        return _state == GLFW_PRESS;
    }

    bool Input::isMouseReleased(MouseCode _button) {
        auto _window = static_cast<GLFWwindow*>(Engine::get().getWindow().getNativeWindow());
        auto _state = glfwGetMouseButton(_window, static_cast<int32_t>(_button));
        return _state == GLFW_RELEASE;
    }

    Vec2I Input::getMousePosition() {
        auto _window = static_cast<GLFWwindow*>(Engine::get().getWindow().getNativeWindow());
        int _height, _width;
        glfwGetWindowSize(_window, &_width, &_height);

        double _xPos, _yPos;
        glfwGetCursorPos(_window, &_xPos, &_yPos);
        return { (int)_xPos, (int)(_height - _yPos) };
    }

    int Input::getMouseX() {
        return getMousePosition().x;
    }

    int Input::getMouseY() {
        return getMousePosition().y;
    }
}