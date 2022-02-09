// Created by borja on 6/2/22.


#include "core/render/window/input/MouseInput.h"
#include "core/render/window/input/Input.h"
#include "core/render/window/event/MouseEvent.h"

namespace engine {


    void MouseInput::init(Window* _window) {
        window = _window;

        events[SystemEventEnum::MOUSE_MOVED_E] = BIND_FUNC_1(MouseInput::onMouseMoved);
        events[SystemEventEnum::MOUSE_DOWN_E] = BIND_FUNC_1(MouseInput::onMouseDown);
        events[SystemEventEnum::MOUSE_UP_E] = BIND_FUNC_1(MouseInput::onMouseUp);
        events[SystemEventEnum::MOUSE_SCROLLED_E] = BIND_FUNC_1(MouseInput::onMouseScroll);

        pressedMouseButtons = {
                {MouseCode::Button0,        0},
                {MouseCode::Button1,        0},
                {MouseCode::Button2,        0},
                {MouseCode::Button3,        0},
                {MouseCode::Button4,        0},
                {MouseCode::Button5,        0},
                {MouseCode::Button6,        0},
                {MouseCode::Button7,        0},
                {MouseCode::ButtonLast,     0},
                {MouseCode::ButtonLeft,     0},
                {MouseCode::ButtonMiddle,   0},
                {MouseCode::ButtonRight,    0},
        };
    }

    bool MouseInput::pollEvent(SDL_Event& _event) {
        if(events.find((SystemEventEnum)_event.type) == events.end()) {
            return false;
        }

        events[(int)_event.type](_event);
        return true;
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
        pressedMouseButtons[_key] = 0;

        MouseButtonReleasedEvent _e(_key);
        window->consumeEvent(_e);
    }

    void MouseInput::onMouseScroll(SDL_Event& _event) {
        MouseScrolledEvent _e(_event.wheel.x, _event.wheel.y);
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