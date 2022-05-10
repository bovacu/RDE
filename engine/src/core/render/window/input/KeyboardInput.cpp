// Created by borja on 6/2/22.


#include "core/render/window/input/KeyboardInput.h"
#include "core/render/window/event/KeyEvent.h"

namespace GDE {


    void KeyboardInput::init(Window* _window) {
        window = _window;

        UDelegate<void(SDL_Event&, RmlData*)> kdeDel, kueDel;
        kdeDel.bind<&KeyboardInput::onKeyDown>(this);
        kueDel.bind<&KeyboardInput::onKeyUp>(this);

        events[SystemEventEnum::KEY_DOWN_E] = kdeDel;
        events[SystemEventEnum::KEY_UP_E] = kueDel;

        pressedKeyboardKeys = {
                {KeyCode::LeftControl,  0},
                {KeyCode::LeftAlt,      0},
                {KeyCode::Space,        0},
                {KeyCode::RightAlt,     0},
                {KeyCode::RightControl, 0},
                {KeyCode::LeftShift,    0},
                {KeyCode::Z,            0},
                {KeyCode::X,            0},
                {KeyCode::C,            0},
                {KeyCode::V,            0},
                {KeyCode::B,            0},
                {KeyCode::N,            0},
                {KeyCode::M,            0},
                {KeyCode::Comma,        0},
                {KeyCode::Period,       0},
                {KeyCode::Slash,        0},
                {KeyCode::RightShift,   0},
                {KeyCode::CapsLock,     0},
                {KeyCode::A,            0},
                {KeyCode::S,            0},
                {KeyCode::D,            0},
                {KeyCode::F,            0},
                {KeyCode::G,            0},
                {KeyCode::H,            0},
                {KeyCode::J,            0},
                {KeyCode::K,            0},
                {KeyCode::L,            0},
                {KeyCode::Semicolon,    0},
                {KeyCode::Apostrophe,   0},
                {KeyCode::Enter,        0},
                {KeyCode::Tab,          0},
                {KeyCode::Q,            0},
                {KeyCode::W,            0},
                {KeyCode::E,            0},
                {KeyCode::R,            0},
                {KeyCode::T,            0},
                {KeyCode::Y,            0},
                {KeyCode::U,            0},
                {KeyCode::I,            0},
                {KeyCode::O,            0},
                {KeyCode::P,            0},
                {KeyCode::LeftBracket,  0},
                {KeyCode::RightBracket, 0},
                {KeyCode::Backslash,    0},
                {KeyCode::GraveAccent,  0},
                {KeyCode::D0,           0},
                {KeyCode::D1,           0},
                {KeyCode::D2,           0},
                {KeyCode::D3,           0},
                {KeyCode::D4,           0},
                {KeyCode::D5,           0},
                {KeyCode::D6,           0},
                {KeyCode::D7,           0},
                {KeyCode::D8,           0},
                {KeyCode::D9,           0},
                {KeyCode::Minus,        0},
                {KeyCode::Equal,        0},
                {KeyCode::Backspace,    0},
                {KeyCode::Escape,       0},
                {KeyCode::F1,           0},
                {KeyCode::F2,           0},
                {KeyCode::F3,           0},
                {KeyCode::F4,           0},
                {KeyCode::F5,           0},
                {KeyCode::F6,           0},
                {KeyCode::F7,           0},
                {KeyCode::F8,           0},
                {KeyCode::F9,           0},
                {KeyCode::F10,          0},
                {KeyCode::F11,          0},
                {KeyCode::F12,          0},
                {KeyCode::PrintScreen,  0},
                {KeyCode::ScrollLock,   0},
                {KeyCode::Insert,       0},
                {KeyCode::Delete,       0},
                {KeyCode::Home,         0},
                {KeyCode::End,          0},
                {KeyCode::PageUp,       0},
                {KeyCode::PageDown,     0},
                {KeyCode::Right,        0},
                {KeyCode::Left,         0},
                {KeyCode::Up,           0},
                {KeyCode::Down,         0},

        };

        ignoredEvents = { KEY_MAP_CHANGED_E, KEY_TEXT_INPUT_E, KEY_TEXT_EDITING_E };
    }

    void KeyboardInput::onKeyDown(SDL_Event& _event, RmlData* _rmlData) {
        auto _key = static_cast<KeyCode>(_event.key.keysym.scancode);

        KeyPressedEvent _e(_key, 1);
        window->consumeEvent(_e);

        if(pressedKeyboardKeys[_key] == 2) return;
            pressedKeyboardKeys[_key] = 1;
    }

    void KeyboardInput::onKeyUp(SDL_Event& _event, RmlData* _rmlData) {
        auto _key = static_cast<KeyCode>(_event.key.keysym.scancode);
        pressedKeyboardKeys[_key] = 0;

        KeyReleasedEvent _e(_key);
        window->consumeEvent(_e);
    }

    int KeyboardInput::getState(int _keyOrButton) {
        return pressedKeyboardKeys[(KeyCode)_keyOrButton];
    }

    void KeyboardInput::setState(int _keyOrButton, int _state) {
        pressedKeyboardKeys[(KeyCode)_keyOrButton] = _state;
    }
}