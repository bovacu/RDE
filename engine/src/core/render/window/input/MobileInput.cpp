// Created by borja on 10/2/22.

#include "core/render/window/input/MobileInput.h"
#include "core/render/window/event/MobileEvent.h"

namespace GDE {


    void MobileInput::init(Window* _window) {
        window = _window;

        UDelegate<void(SDL_Event&)> otdDel, otuDel, otmDel;
        otdDel.bind<&MobileInput::onTouchDown>(this);
        otuDel.bind<&MobileInput::onTouchUp>(this);
        otmDel.bind<&MobileInput::onTouchMoved>(this);

        events[SystemEventEnum::MOBILE_TOUCH_DOWN_E] = otdDel;
        events[SystemEventEnum::MOBILE_TOUCH_UP_E] = otuDel;
        events[SystemEventEnum::MOBILE_TOUCH_MOVED] = otmDel;

        for(int _i = 0; _i < 20; _i++)
            pressedFingers[_i] = -1;
    }

    int MobileInput::getState(int _keyOrButton) {
        return pressedFingers[_keyOrButton];
    }

    void MobileInput::setState(int _keyOrButton, int _state) {
        pressedFingers[_keyOrButton] = _state;
    }

    void MobileInput::onTouchDown(SDL_Event& _event) {
        pressure = _event.tfinger.pressure;
        fingerPosInit = {(float)window->getWindowSize().x * _event.tfinger.x, (float)window->getWindowSize().y * _event.tfinger.y};

        MobileTouchDownEvent _e(fingerPosInit, pressure, _event.tfinger.timestamp, (int)_event.tfinger.fingerId);
        window->consumeEvent(_e);

        if(pressedFingers[(int)_event.tfinger.fingerId] == 2) return;
            pressedFingers[(int)_event.tfinger.fingerId] = 1;
    }

    void MobileInput::onTouchUp(SDL_Event& _event) {
        pressure = 0;
        fingerPosEnd = {(float)window->getWindowSize().x * _event.tfinger.x, (float)window->getWindowSize().y * _event.tfinger.y};
        initOfTouch = _event.tfinger.timestamp;

        MobileTouchUpEvent _e(fingerPosEnd, _event.tfinger.timestamp, (int)_event.tfinger.fingerId);
        window->consumeEvent(_e);

        pressedFingers[(int)_event.tfinger.fingerId] = 0;
    }

    void MobileInput::onTouchMoved(SDL_Event& _event) {
        Vec2F _init = {(float)window->getWindowSize().x * _event.tfinger.x, (float)window->getWindowSize().y * _event.tfinger.y};
        Vec2F _end = {(float)window->getWindowSize().x * _event.tfinger.dx, (float)window->getWindowSize().y * _event.tfinger.dy};

        endOfTouch = _event.tfinger.timestamp;

        MobileTouchMovedEvent _e(_init, _end);
        window->consumeEvent(_e);
    }
}

