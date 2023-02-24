// Created by borja on 6/2/22.


#include "core/systems/inputSystem/input/KeyboardInput.h"
#include "core/systems/eventSystem/KeyEvent.h"
#include "core/systems/inputSystem/input/Input.h"
#include "core/util/Functions.h"
#include <SDL_events.h>
#include <stdio.h>

namespace RDE {


    void KeyboardInput::init(Engine* _engine, Window* _window) {
        engine = _engine;
        window = _window;

        UniqueDelegate<void(SDL_Event&)> kdeDel, kueDel, tteDel;
        kdeDel.bind<&KeyboardInput::onKeyDown>(this);
        kueDel.bind<&KeyboardInput::onKeyUp>(this);
        tteDel.bind<&KeyboardInput::onTextTyped>(this);

        events[RDE_SYSTEM_EVENT_KEY_DOWN_E] = kdeDel;
		events[RDE_SYSTEM_EVENT_KEY_UP_E] = kueDel;
		events[RDE_SYSTEM_EVENT_KEY_TEXT_INPUT_E] = tteDel;

        pressedKeyboardKeys = {
			{RDE_KEYBOARD_KEY_LEFT_CTRL,  		0},
				{RDE_KEYBOARD_KEY_LEFT_ALT,     0},
				{RDE_KEYBOARD_KEY_SPACE,        0},
				{RDE_KEYBOARD_KEY_RIGHT_ALT,    0},
				{RDE_KEYBOARD_KEY_RIGHT_CTRL, 	0},
				{RDE_KEYBOARD_KEY_LEFT_SHIFT,   0},
				{RDE_KEYBOARD_KEY_Z,            0},
				{RDE_KEYBOARD_KEY_X,            0},
				{RDE_KEYBOARD_KEY_C,            0},
				{RDE_KEYBOARD_KEY_V,            0},
				{RDE_KEYBOARD_KEY_B,            0},
				{RDE_KEYBOARD_KEY_N,            0},
				{RDE_KEYBOARD_KEY_M,            0},
				{RDE_KEYBOARD_KEY_COMMA,        0},
				{RDE_KEYBOARD_KEY_PERIOD,       0},
				{RDE_KEYBOARD_KEY_SLASH,        0},
				{RDE_KEYBOARD_KEY_RIGHT_SHIFT,  0},
				{RDE_KEYBOARD_KEY_CAPS_LOCK,    0},
				{RDE_KEYBOARD_KEY_A,            0},
				{RDE_KEYBOARD_KEY_S,            0},
				{RDE_KEYBOARD_KEY_D,            0},
				{RDE_KEYBOARD_KEY_F,            0},
				{RDE_KEYBOARD_KEY_G,            0},
				{RDE_KEYBOARD_KEY_H,            0},
				{RDE_KEYBOARD_KEY_J,            0},
				{RDE_KEYBOARD_KEY_K,            0},
				{RDE_KEYBOARD_KEY_L,            0},
				{RDE_KEYBOARD_KEY_SEMICOLON,    0},
				{RDE_KEYBOARD_KEY_APOSTROPHE,   0},
				{RDE_KEYBOARD_KEY_ENTER,        0},
				{RDE_KEYBOARD_KEY_TAB,          0},
				{RDE_KEYBOARD_KEY_Q,            0},
				{RDE_KEYBOARD_KEY_W,            0},
				{RDE_KEYBOARD_KEY_E,            0},
				{RDE_KEYBOARD_KEY_R,            0},
				{RDE_KEYBOARD_KEY_T,            0},
				{RDE_KEYBOARD_KEY_Y,            0},
				{RDE_KEYBOARD_KEY_U,            0},
				{RDE_KEYBOARD_KEY_I,            0},
				{RDE_KEYBOARD_KEY_O,            0},
				{RDE_KEYBOARD_KEY_P,            0},
				{RDE_KEYBOARD_KEY_LEFT_BRACKET, 0},
				{RDE_KEYBOARD_KEY_RIGHT_BRACKET,0},
				{RDE_KEYBOARD_KEY_BACK_SLASH,   0},
				{RDE_KEYBOARD_KEY_GRACE_ACCENT, 0},
				{RDE_KEYBOARD_KEY_D0,           0},
				{RDE_KEYBOARD_KEY_D1,           0},
				{RDE_KEYBOARD_KEY_D2,           0},
				{RDE_KEYBOARD_KEY_D3,           0},
				{RDE_KEYBOARD_KEY_D4,           0},
				{RDE_KEYBOARD_KEY_D5,           0},
				{RDE_KEYBOARD_KEY_D6,           0},
				{RDE_KEYBOARD_KEY_D7,           0},
				{RDE_KEYBOARD_KEY_D8,           0},
				{RDE_KEYBOARD_KEY_D9,           0},
				{RDE_KEYBOARD_KEY_MINUS,        0},
				{RDE_KEYBOARD_KEY_EQUAL,        0},
				{RDE_KEYBOARD_KEY_BAKCSPACE,    0},
				{RDE_KEYBOARD_KEY_ESCAPE,       0},
				{RDE_KEYBOARD_KEY_F1,           0},
				{RDE_KEYBOARD_KEY_F2,           0},
				{RDE_KEYBOARD_KEY_F3,           0},
				{RDE_KEYBOARD_KEY_F4,           0},
				{RDE_KEYBOARD_KEY_F5,           0},
				{RDE_KEYBOARD_KEY_F6,           0},
				{RDE_KEYBOARD_KEY_F7,           0},
				{RDE_KEYBOARD_KEY_F8,           0},
				{RDE_KEYBOARD_KEY_F9,           0},
				{RDE_KEYBOARD_KEY_F10,          0},
				{RDE_KEYBOARD_KEY_F11,          0},
				{RDE_KEYBOARD_KEY_F12,          0},
				{RDE_KEYBOARD_KEY_PRINT_SCREEN, 0},
				{RDE_KEYBOARD_KEY_SCROLL_LOCK,  0},
				{RDE_KEYBOARD_KEY_INSERT,       0},
				{RDE_KEYBOARD_KEY_DELETE,       0},
				{RDE_KEYBOARD_KEY_HOME,         0},
				{RDE_KEYBOARD_KEY_END,          0},
				{RDE_KEYBOARD_KEY_PAGE_UP,      0},
				{RDE_KEYBOARD_KEY_PAGE_DOWN,    0},
				{RDE_KEYBOARD_KEY_RIGHT,        0},
				{RDE_KEYBOARD_KEY_LEFT,         0},
				{RDE_KEYBOARD_KEY_UP,           0},
				{RDE_KEYBOARD_KEY_DOWN,         0},

        };

		ignoredEvents = { RDE_SYSTEM_EVENT_KEY_MAP_CHANGED_E, RDE_SYSTEM_EVENT_KEY_TEXT_EDITING_E };
    }

    void KeyboardInput::onKeyDown(SDL_Event& _event) {
		auto _key = static_cast<RDE_KEYBOARD_KEY_>(_event.key.keysym.scancode);

        KeyPressedEvent _e(_key, 1);
        window->consumeEvent(_e);

        if(pressedKeyboardKeys[_key] == 2) return;
            pressedKeyboardKeys[_key] = 1;
    }

    void KeyboardInput::onKeyUp(SDL_Event& _event) {
		auto _key = static_cast<RDE_KEYBOARD_KEY_>(_event.key.keysym.scancode);
        pressedKeyboardKeys[_key] = 0;

        KeyReleasedEvent _e(_key);
        window->consumeEvent(_e);
    }

    void KeyboardInput::onTextTyped(SDL_Event& _event) {
        TextTypedEvent _e(_event.text.text);
        window->consumeEvent(_e);
    }

    int KeyboardInput::getState(int _keyOrButton) {
        return pressedKeyboardKeys[(RDE_KEYBOARD_KEY_)_keyOrButton];
    }

    void KeyboardInput::setState(int _keyOrButton, int _state) {
		pressedKeyboardKeys[(RDE_KEYBOARD_KEY_)_keyOrButton] = _state;
    }
}