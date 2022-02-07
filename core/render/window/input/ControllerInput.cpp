// Created by borja on 6/2/22.


#include "ControllerInput.h"
#include "Input.h"
#include "core/render/window/event/JoystickEvent.h"

namespace engine {


    void ControllerInput::init(Window* _window) {
        window = _window;

        events[SystemEventEnum::GAMEPAD_JOYSTICK] = BIND_FUNC_1(ControllerInput::onGamepadsMoved);
        events[SystemEventEnum::GAMEPAD_BUTTON_DOWN] = BIND_FUNC_1(ControllerInput::onGamepadsButtonDown);
        events[SystemEventEnum::GAMEPAD_BUTTON_UP] = BIND_FUNC_1(ControllerInput::onGamepadsButtonUp);
        events[SystemEventEnum::GAMEPAD_CONNECTED_E] = BIND_FUNC_1(ControllerInput::onGamepadConnected);
        events[SystemEventEnum::GAMEPAD_DISCONNECTED_E] = BIND_FUNC_1(ControllerInput::onGamepadDisconnected);

        pressedGamepadKeys = {
                {GamePadKeys::ButtonA,      0},
                {GamePadKeys::ButtonB,      0},
                {GamePadKeys::ButtonX,      0},
                {GamePadKeys::ButtonY,      0},
                {GamePadKeys::DPadDown,     0},
                {GamePadKeys::DPadLeft,     0},
                {GamePadKeys::DPadRight,    0},
                {GamePadKeys::DPadUp,       0},
                {GamePadKeys::Guide,        0},
                {GamePadKeys::LB,           0},
                {GamePadKeys::RB,           0},
                {GamePadKeys::LeftJoystick, 0},
                {GamePadKeys::RightJoystick,0},
                {GamePadKeys::Select,       0},
                {GamePadKeys::Start,        0},
        };

        initGamepads();
    }

    bool ControllerInput::pollEvent(SDL_Event& _event) {
        return false;
    }

    void ControllerInput::initGamepads() {
        int _numJoysticks = SDL_NumJoysticks();
        if(_numJoysticks < 1) {
            LOG_I("No joysticks connected")
            return;
        }
        for(int _i = 0; _i < _numJoysticks; _i++) {
            SDL_Joystick* _currentJoystick = SDL_JoystickOpen(_i);
            if(_currentJoystick == nullptr) {
                LOG_E("Error with joystick ", _i)
                continue;
            }

            joysticks.push_back(_currentJoystick);
        }

        LOG_I(_numJoysticks, " connected and ", joysticks.size(), " loaded")
    }

    void ControllerInput::onGamepadsMoved(SDL_Event& _event) {
        Vec2F _left;
        if(_event.caxis.axis == 0) _left.x = (float)_event.caxis.value / (float)SDL_JOYSTICK_AXIS_MAX;
        if(_event.caxis.axis == 1) _left.y = (float)_event.caxis.value / (float)SDL_JOYSTICK_AXIS_MAX;

        Vec2F _right;
        if(_event.caxis.axis == 2) _right.x = (float)_event.caxis.value / (float)SDL_JOYSTICK_AXIS_MAX;
        if(_event.caxis.axis == 3) _right.y = (float)_event.caxis.value / (float)SDL_JOYSTICK_AXIS_MAX;

        Vec2F _back;
        if(_event.caxis.axis == 4) _back.x = (float)_event.caxis.value / (float)SDL_JOYSTICK_AXIS_MAX;
        if(_event.caxis.axis == 5) _back.y = (float)_event.caxis.value / (float)SDL_JOYSTICK_AXIS_MAX;

        JoystickAxisMovedEvent _e(_left, _right, _back);
        window->consumeEvent(_e);

        leftJoystickValue = _left;
        rightJoystickValue = _right;
        backButtonsValue = _back;
    }

    void ControllerInput::onGamepadsButtonDown(SDL_Event& _event) {
        auto _key = static_cast<GamePadKeys>(_event.cbutton.button);

        JoystickButtonDownEvent _e(_key);
        window->consumeEvent(_e);

        if(pressedGamepadKeys[_key] == 2) return;
            pressedGamepadKeys[_key] = 1;
    }

    void ControllerInput::onGamepadsButtonUp(SDL_Event& _event) {
        auto _key = static_cast<GamePadKeys>(_event.cbutton.button);
        pressedGamepadKeys[_key] = 0;

        JoystickButtonUpEvent _e(_key);
        window->consumeEvent(_e);
    }

    void ControllerInput::onGamepadConnected(SDL_Event& _event) {
        LOG_W("Added controller: ", _event.cdevice.which)
    }

    void ControllerInput::onGamepadDisconnected(SDL_Event& _event) {
        LOG_W("Removed controller: ", _event.cdevice.which)
    }
}