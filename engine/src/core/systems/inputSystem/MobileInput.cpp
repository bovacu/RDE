// Created by borja on 10/2/22.

#include "core/systems/inputSystem/input/MobileInput.h"
#include "core/systems/eventSystem/MobileEvent.h"

namespace RDE {


    void MobileInput::init(Engine* _engine, Window* _window) {
        engine = _engine;
        window = _window;

        UniqueDelegate<void(SDL_Event&)> otdDel, otuDel, otmDel;
        otdDel.bind<&MobileInput::onTouchDown>(this);
        otuDel.bind<&MobileInput::onTouchUp>(this);
        otmDel.bind<&MobileInput::onTouchMoved>(this);

        events[RDE_SYSTEM_EVENT_MOBILE_TOUCH_DOWN_E] = otdDel;
        events[RDE_SYSTEM_EVENT_MOBILE_TOUCH_UP_E] = otuDel;
        events[RDE_SYSTEM_EVENT_MOBILE_TOUCH_MOVED] = otmDel;

        for(int _i = 0; _i < 20; _i++)
			pressedFingers[_i] = RDE_INPUT_STATUS_KEEP_RELEASED;
    }

	RDE_INPUT_STATUS_ MobileInput::getState(int _finger) {
		return pressedFingers[_finger];
    }

	void MobileInput::setState(int _finger, RDE_INPUT_STATUS_ _state) {
		pressedFingers[_finger] = _state;
    }

    void MobileInput::onTouchDown(SDL_Event& _event) {
        pressure = _event.tfinger.pressure;
        fingerPosInit = {(float)window->getWindowSize().x * _event.tfinger.x, (float)window->getWindowSize().y * _event.tfinger.y};

		pressedFingers[(int)_event.tfinger.fingerId] = RDE_INPUT_STATUS_JUST_PRESSED;

        MobileTouchDownEvent _e(fingerPosInit, pressure, _event.tfinger.timestamp, (int)_event.tfinger.fingerId);
        window->consumeEvent(_e); 
    }

    void MobileInput::onTouchUp(SDL_Event& _event) {
        pressure = 0;
        fingerPosEnd = {(float)window->getWindowSize().x * _event.tfinger.x, (float)window->getWindowSize().y * _event.tfinger.y};
        initOfTouch = _event.tfinger.timestamp;

		pressedFingers[(int)_event.tfinger.fingerId] = RDE_INPUT_STATUS_JUST_RELEASED;

        MobileTouchUpEvent _e(fingerPosEnd, _event.tfinger.timestamp, (int)_event.tfinger.fingerId);
        window->consumeEvent(_e);
    }

    void MobileInput::onTouchMoved(SDL_Event& _event) {
        Vec2F _init = {(float)window->getWindowSize().x * _event.tfinger.x, (float)window->getWindowSize().y * _event.tfinger.y};
        Vec2F _end = {(float)window->getWindowSize().x * _event.tfinger.dx, (float)window->getWindowSize().y * _event.tfinger.dy};

        endOfTouch = _event.tfinger.timestamp;

        MobileTouchMovedEvent _e(_init, _end);
        window->consumeEvent(_e);
    }
}

