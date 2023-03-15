// Created by borja on 6/2/22.


#include "core/systems/inputSystem/input/MouseInput.h"
#include "core/systems/eventSystem/MouseEvent.h"

namespace RDE {


    void MouseInput::init(Engine* _engine, Window* _window) {
        engine = _engine;
        window = _window;

        UniqueDelegate<void(SDL_Event&)> mmeDel, mdeDel, mueDel, mseDel;
        mmeDel.bind<&MouseInput::onMouseMoved>(this);
        mdeDel.bind<&MouseInput::onMouseDown>(this);
        mueDel.bind<&MouseInput::onMouseUp>(this);
        mseDel.bind<&MouseInput::onMouseScroll>(this);

        events[RDE_SYSTEM_EVENT_MOUSE_MOVED_E] = mmeDel;
        events[RDE_SYSTEM_EVENT_MOUSE_DOWN_E] = mdeDel;
        events[RDE_SYSTEM_EVENT_MOUSE_UP_E] = mueDel;
        events[RDE_SYSTEM_EVENT_MOUSE_SCROLLED_E] = mseDel;

        pressedMouseButtons = {
			{ RDE_MOUSE_BUTTON_0,        RDE_INPUT_STATUS_KEEP_RELEASED },
			{ RDE_MOUSE_BUTTON_1,        RDE_INPUT_STATUS_KEEP_RELEASED },
			{ RDE_MOUSE_BUTTON_2,        RDE_INPUT_STATUS_KEEP_RELEASED },
			{ RDE_MOUSE_BUTTON_3,        RDE_INPUT_STATUS_KEEP_RELEASED },
			{ RDE_MOUSE_BUTTON_4,        RDE_INPUT_STATUS_KEEP_RELEASED },
			{ RDE_MOUSE_BUTTON_5,        RDE_INPUT_STATUS_KEEP_RELEASED },
			{ RDE_MOUSE_BUTTON_6,        RDE_INPUT_STATUS_KEEP_RELEASED },
			{ RDE_MOUSE_BUTTON_7,        RDE_INPUT_STATUS_KEEP_RELEASED },
			{ RDE_MOUSE_BUTTON_LAST,     RDE_INPUT_STATUS_KEEP_RELEASED },
			{ RDE_MOUSE_BUTTON_LEFT,     RDE_INPUT_STATUS_KEEP_RELEASED },
			{ RDE_MOUSE_BUTTON_MIDDLE,   RDE_INPUT_STATUS_KEEP_RELEASED },
			{ RDE_MOUSE_BUTTON_RIGHT,    RDE_INPUT_STATUS_KEEP_RELEASED },
        };
    }

    void MouseInput::onMouseMoved(SDL_Event& _event) {
        mousePos = {_event.motion.x, _event.motion.y};
        MouseMovedEvent _e((float)_event.motion.x, (float)_event.motion.y);
        window->consumeEvent(_e);
    }

    void MouseInput::onMouseDown(SDL_Event& _event) {
		auto _key = static_cast<RDE_MOUSE_BUTTON_>(_event.button.button);

		pressedMouseButtons[_key] = RDE_INPUT_STATUS_JUST_PRESSED;

        MouseButtonPressedEvent _e(_key);
        window->consumeEvent(_e);
    }

    void MouseInput::onMouseUp(SDL_Event& _event) {
		auto _key = static_cast<RDE_MOUSE_BUTTON_>(_event.button.button);

		pressedMouseButtons[_key] = RDE_INPUT_STATUS_JUST_RELEASED;

        MouseButtonReleasedEvent _e(_key);
        window->consumeEvent(_e);
    }

    void MouseInput::onMouseScroll(SDL_Event& _event) {
        MouseScrolledEvent _e((float)_event.wheel.x, (float)_event.wheel.y);
        window->consumeEvent(_e);
    }

	RDE_INPUT_STATUS_ MouseInput::getState(RDE_MOUSE_BUTTON_ _keyOrButton) {
		return pressedMouseButtons[_keyOrButton];
    }

	void MouseInput::setState(RDE_MOUSE_BUTTON_ _keyOrButton, RDE_INPUT_STATUS_ _state) {
		pressedMouseButtons[_keyOrButton] = _state;
    }

    Vec2F MouseInput::getMousePosition() const {
        return {(float)mousePos.x, (float)mousePos.y};
    }
}