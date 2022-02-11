// Created by borja on 6/2/22.


#include "core/render/window/input/KeyboardInput.h"
#include "core/render/window/event/KeyEvent.h"

namespace engine {


    void KeyboardInput::init(Window* _window) {
        window = _window;

        events[SystemEventEnum::KEY_DOWN_E] = BIND_FUNC_1(KeyboardInput::onKeyDown);
        events[SystemEventEnum::KEY_UP_E] = BIND_FUNC_1(KeyboardInput::onKeyUp);

        pressedKeyboardKeys = {
                {KeyCode::LeftControl,  -1},
                {KeyCode::LeftAlt,      -1},
                {KeyCode::Space,        -1},
                {KeyCode::RightAlt,     -1},
                {KeyCode::RightControl, -1},
                {KeyCode::LeftShift,    -1},
                {KeyCode::Z,            -1},
                {KeyCode::X,            -1},
                {KeyCode::C,            -1},
                {KeyCode::V,            -1},
                {KeyCode::B,            -1},
                {KeyCode::N,            -1},
                {KeyCode::M,            -1},
                {KeyCode::Comma,        -1},
                {KeyCode::Period,       -1},
                {KeyCode::Slash,        -1},
                {KeyCode::RightShift,   -1},
                {KeyCode::CapsLock,     -1},
                {KeyCode::A,            -1},
                {KeyCode::S,            -1},
                {KeyCode::D,            -1},
                {KeyCode::F,            -1},
                {KeyCode::G,            -1},
                {KeyCode::H,            -1},
                {KeyCode::J,            -1},
                {KeyCode::K,            -1},
                {KeyCode::L,            -1},
                {KeyCode::Semicolon,    -1},
                {KeyCode::Apostrophe,   -1},
                {KeyCode::Enter,        -1},
                {KeyCode::Tab,          -1},
                {KeyCode::Q,            -1},
                {KeyCode::W,            -1},
                {KeyCode::E,            -1},
                {KeyCode::R,            -1},
                {KeyCode::T,            -1},
                {KeyCode::Y,            -1},
                {KeyCode::U,            -1},
                {KeyCode::I,            -1},
                {KeyCode::O,            -1},
                {KeyCode::P,            -1},
                {KeyCode::LeftBracket,  -1},
                {KeyCode::RightBracket, -1},
                {KeyCode::Backslash,    -1},
                {KeyCode::GraveAccent,  -1},
                {KeyCode::D0,           -1},
                {KeyCode::D1,           -1},
                {KeyCode::D2,           -1},
                {KeyCode::D3,           -1},
                {KeyCode::D4,           -1},
                {KeyCode::D5,           -1},
                {KeyCode::D6,           -1},
                {KeyCode::D7,           -1},
                {KeyCode::D8,           -1},
                {KeyCode::D9,           -1},
                {KeyCode::Minus,        -1},
                {KeyCode::Equal,        -1},
                {KeyCode::Backspace,    -1},
                {KeyCode::Escape,       -1},
                {KeyCode::F1,           -1},
                {KeyCode::F2,           -1},
                {KeyCode::F3,           -1},
                {KeyCode::F4,           -1},
                {KeyCode::F5,           -1},
                {KeyCode::F6,           -1},
                {KeyCode::F7,           -1},
                {KeyCode::F8,           -1},
                {KeyCode::F9,           -1},
                {KeyCode::F10,          -1},
                {KeyCode::F11,          -1},
                {KeyCode::F12,          -1},
                {KeyCode::PrintScreen,  -1},
                {KeyCode::ScrollLock,   -1},
                {KeyCode::Insert,       -1},
                {KeyCode::Delete,       -1},
                {KeyCode::Home,         -1},
                {KeyCode::End,          -1},
                {KeyCode::PageUp,       -1},
                {KeyCode::PageDown,     -1},
                {KeyCode::Right,        -1},
                {KeyCode::Left,         -1},
                {KeyCode::Up,           -1},
                {KeyCode::Down,         -1},

        };

        ignoredEvents = { KEY_MAP_CHANGED_E, KEY_TEXT_INPUT_E, KEY_TEXT_EDITING_E };
    }

    void KeyboardInput::onKeyDown(SDL_Event& _event) {
        auto _key = static_cast<KeyCode>(_event.key.keysym.scancode);

        KeyPressedEvent _e(_key, 1);
        window->consumeEvent(_e);

        if(pressedKeyboardKeys[_key] == 2) return;
            pressedKeyboardKeys[_key] = 1;
    }

    void KeyboardInput::onKeyUp(SDL_Event& _event) {
        auto _key = static_cast<KeyCode>(_event.key.keysym.scancode);

        KeyReleasedEvent _e(_key);
        window->consumeEvent(_e);

        pressedKeyboardKeys[_key] = 0;
    }

    int KeyboardInput::getState(int _keyOrButton) {
        return pressedKeyboardKeys[(KeyCode)_keyOrButton];
    }

    void KeyboardInput::setState(int _keyOrButton, int _state) {
        pressedKeyboardKeys[(KeyCode)_keyOrButton] = _state;
    }
}