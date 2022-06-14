// Created by borja on 6/2/22.


#include "core/systems/inputSystem/input/MouseInput.h"
#include "engine/include/core/systems/eventSystem/MouseEvent.h"

namespace GDE {


    void MouseInput::init(Engine* _engine, Window* _window) {
        engine = _engine;
        window = _window;

        UDelegate<void(SDL_Event&)> mmeDel, mdeDel, mueDel, mseDel;
        mmeDel.bind<&MouseInput::onMouseMoved>(this);
        mdeDel.bind<&MouseInput::onMouseDown>(this);
        mueDel.bind<&MouseInput::onMouseUp>(this);
        mseDel.bind<&MouseInput::onMouseScroll>(this);

        events[SystemEventEnum::MOUSE_MOVED_E] = mmeDel;
        events[SystemEventEnum::MOUSE_DOWN_E] = mdeDel;
        events[SystemEventEnum::MOUSE_UP_E] = mueDel;
        events[SystemEventEnum::MOUSE_SCROLLED_E] = mseDel;

        pressedMouseButtons = {
                {MouseCode::Button0,        -1},
                {MouseCode::Button1,        -1},
                {MouseCode::Button2,        -1},
                {MouseCode::Button3,        -1},
                {MouseCode::Button4,        -1},
                {MouseCode::Button5,        -1},
                {MouseCode::Button6,        -1},
                {MouseCode::Button7,        -1},
                {MouseCode::ButtonLast,     -1},
                {MouseCode::ButtonLeft,     -1},
                {MouseCode::ButtonMiddle,   -1},
                {MouseCode::ButtonRight,    -1},
        };
    }

    void MouseInput::onMouseMoved(SDL_Event& _event) {
        mousePos = {_event.motion.x, _event.motion.y};
        MouseMovedEvent _e((float)_event.motion.x, (float)_event.motion.y);
        window->consumeEvent(_e);
    }

    void MouseInput::onMouseDown(SDL_Event& _event) {
        auto _key = static_cast<MouseCode>(_event.button.button);

        MouseButtonPressedEvent _e(_key);
        window->consumeEvent(_e);

        if(pressedMouseButtons[_key] == 2) return;
        pressedMouseButtons[_key] = 1;
    }

    void MouseInput::onMouseUp(SDL_Event& _event) {
        auto _key = static_cast<MouseCode>(_event.button.button);

        MouseButtonReleasedEvent _e(_key);
        window->consumeEvent(_e);

        pressedMouseButtons[_key] = 0;
    }

    void MouseInput::onMouseScroll(SDL_Event& _event) {
        MouseScrolledEvent _e((float)_event.wheel.x, (float)_event.wheel.y);
        window->consumeEvent(_e);
    }

    int MouseInput::getState(int _keyOrButton) {
        return pressedMouseButtons[(MouseCode)_keyOrButton];
    }

    void MouseInput::setState(int _keyOrButton, int _state) {
        pressedMouseButtons[(MouseCode)_keyOrButton] = _state;
    }

    Vec2F MouseInput::getMousePosition() const {
        return {(float)mousePos.x, (float)mousePos.y};
    }
}