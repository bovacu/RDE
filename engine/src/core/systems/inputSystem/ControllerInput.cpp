// Created by borja on 6/2/22.


#include "core/systems/inputSystem/input/ControllerInput.h"
#include "engine/include/core/systems/eventSystem/JoystickEvent.h"

namespace GDE {

    VibrationManager& VibrationManager::get() {
        static VibrationManager _vibrationManager;
        return _vibrationManager;
    }

    void VibrationManager::init(std::unordered_map<int, Controller*>* _controllers) {
        controllers = _controllers;
        addVibrationEffect("default", {
            500,
            0,
            0,
            15000,
            1000
        });
    }

    void VibrationManager::addVibrationEffect(const std::string& _effectName, const VibrationConfig& _config) {
        SDL_HapticEffect _effect;

        SDL_memset( &_effect, 0, sizeof(SDL_HapticEffect) );
        _effect.type = SDL_HAPTIC_SINE;
        _effect.periodic.direction.type = SDL_HAPTIC_CARTESIAN; // Polar coordinates
        _effect.periodic.direction.dir[0] = 18000; // Force comes from south
        _effect.periodic.period = _config.period; // 1000 ms
        _effect.periodic.magnitude = (short)_config.force; // 15000/32767 strength
        _effect.periodic.length = _config.durationInSeconds; // 1 seconds long
        _effect.periodic.attack_length = _config.delayToGetToMaxForce; // Takes 0 second to get max strength
        _effect.periodic.fade_length = _config.delayToGetToMinForce; // Takes 0 second to fade away
        effects[_effectName] = { _effect };
    }

    void VibrationManager::removeVibrationEffect(const std::string& _effectName) {
        for(auto _controllerID : effects[_effectName].controllerIDs)
            SDL_HapticDestroyEffect( controllers->at(_controllerID)->vibration, effects[_effectName].vibrationEffectID);
        effects.erase(_effectName);
    }

    void VibrationManager::assignVibrationEffectToController(const std::string& _effectName, int _controllerID) {
        effects[_effectName].vibrationEffectID = SDL_HapticNewEffect(controllers->at(_controllerID)->vibration, &effects[_effectName].vibrationEffect);
        effects[_effectName].controllerIDs.emplace_back(_controllerID);
    }

    void VibrationManager::destroy() {
        for(auto& _effect : effects) {
            for(auto _controllerID : _effect.second.controllerIDs)
                SDL_HapticDestroyEffect( controllers->at(_controllerID)->vibration, _effect.second.vibrationEffectID);
        }
    }


    Controller::Controller(int _controllerID) : ID(_controllerID) {

        pressedGamepadButtons = {
                {GamePadButtons::ButtonA,       -1},
                {GamePadButtons::ButtonB,       -1},
                {GamePadButtons::ButtonX,       -1},
                {GamePadButtons::ButtonY,       -1},
                {GamePadButtons::DPadDown,      -1},
                {GamePadButtons::DPadLeft,      -1},
                {GamePadButtons::DPadRight,     -1},
                {GamePadButtons::DPadUp,        -1},
                {GamePadButtons::Guide,         -1},
                {GamePadButtons::LB,            -1},
                {GamePadButtons::RB,            -1},
                {GamePadButtons::LeftJoystick,  -1},
                {GamePadButtons::RightJoystick, -1},
                {GamePadButtons::Select,        -1},
                {GamePadButtons::Start,         -1},
        };

        pressedGamepadAxis = {
                {GamePadAxis::Right,-1},
                {GamePadAxis::Left, -1},
                {GamePadAxis::RT,   -1},
                {GamePadAxis::LT,   -1},
        };
    }

    void ControllerInput::init(Engine* _engine, Window* _window) {
        engine = _engine;
        window = _window;

        UDelegate<void(SDL_Event&)> _gpJ, _gpBD, _gpBU, _gpD, _gpC;
        _gpJ.bind<&ControllerInput::onGamepadsMoved>(this);
        _gpBD.bind<&ControllerInput::onGamepadsButtonDown>(this);
        _gpBU.bind<&ControllerInput::onGamepadsButtonUp>(this);
        _gpC.bind<&ControllerInput::onGamepadConnected>(this);
        _gpD.bind<&ControllerInput::onGamepadDisconnected>(this);

        events[SystemEventEnum::GAMEPAD_JOYSTICK] = _gpJ;
        events[SystemEventEnum::GAMEPAD_BUTTON_DOWN] = _gpBD;
        events[SystemEventEnum::GAMEPAD_BUTTON_UP] = _gpBU;
        events[SystemEventEnum::GAMEPAD_CONNECTED_E] = _gpC;
        events[SystemEventEnum::GAMEPAD_DISCONNECTED_E] = _gpD;

        ignoredEvents = { JOYSTICK_HAT_MOTION_E, JOYSTICK_BALL_MOTION_E, JOYSTICK_BUTTON_DOWN_E, JOYSTICK_BUTTON_UP_E, JOYSTICK_CONNECTED_E,
                        JOYSTICK_DISCONNECTED_E, JOYSTICK_AXIS_MOTION_E };

        VibrationManager::get().init(&controllers);
    }

    void ControllerInput::initGamepads() {
        int _numJoysticks = SDL_NumJoysticks();
        LOG_I("Num of Joysticks: ", _numJoysticks)
        if(_numJoysticks < 1) {
            LOG_I("No joysticks connected")
            return;
        }
        for(int _i = 0; _i < _numJoysticks; _i++) {
            auto* _currentGameController = SDL_GameControllerOpen(_i);
            if(_currentGameController == nullptr) {
                LOG_E("Error with controller ", _i)
                auto _controller = new Controller(-100);
                _controller->sdlGameController = _currentGameController;
                controllers[SDL_JoystickGetDeviceInstanceID(_i)] = _controller;
                continue;
            }

            if(controllers[SDL_JoystickGetDeviceInstanceID(_i)] != nullptr) continue;

            LOG_I("Connected Controller ", _i, ", Device Instance: ", SDL_JoystickGetDeviceInstanceID(_i))
            auto _controller = new Controller(_i);
            _controller->sdlGameController = _currentGameController;
            _controller->vibration = SDL_HapticOpenFromJoystick(SDL_GameControllerGetJoystick(_currentGameController));
            controllers[SDL_JoystickGetDeviceInstanceID(_i)] = _controller;

            VibrationManager::get().assignVibrationEffectToController("default", _controller->ID);
        }

        LOG_I(_numJoysticks, " connected and ", controllers.size(), " loaded")
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

        int _controllerID = _event.cdevice.which;

        controllers[_controllerID]->leftJoystickValue = _left;
        controllers[_controllerID]->rightJoystickValue = _right;
        controllers[_controllerID]->backButtonsValue = _back;

        float _epsilon = 0.001f;

        if(std::abs(_left.x) <= _epsilon && std::abs(_left.y) <= _epsilon)
            controllers[_controllerID]->pressedGamepadAxis[GamePadAxis::Left] = 0;
        else if(controllers[_controllerID]->pressedGamepadAxis[GamePadAxis::Left] != 2)
            controllers[_controllerID]->pressedGamepadAxis[GamePadAxis::Left] = 1;

        if(std::abs(_right.x) <= _epsilon && std::abs(_right.y) <= _epsilon)
            controllers[_controllerID]->pressedGamepadAxis[GamePadAxis::Right] = 0;
        else if(controllers[_controllerID]->pressedGamepadAxis[GamePadAxis::Right] != 2)
            controllers[_controllerID]->pressedGamepadAxis[GamePadAxis::Right] = 1;

        if(std::abs(_back.x) <= _epsilon)
            controllers[_controllerID]->pressedGamepadAxis[GamePadAxis::LT] = 0;
        else if(controllers[_controllerID]->pressedGamepadAxis[GamePadAxis::LT] != 2)
            controllers[_controllerID]->pressedGamepadAxis[GamePadAxis::LT] = 1;

        if(std::abs(_back.y) <= _epsilon)
            controllers[_controllerID]->pressedGamepadAxis[GamePadAxis::RT] = 0;
        else if(controllers[_controllerID]->pressedGamepadAxis[GamePadAxis::RT] != 2)
            controllers[_controllerID]->pressedGamepadAxis[GamePadAxis::RT] = 1;
    }

    void ControllerInput::onGamepadsButtonDown(SDL_Event& _event) {
        auto _key = static_cast<GamePadButtons>(_event.cbutton.button);

        JoystickButtonDownEvent _e(_key);
        window->consumeEvent(_e);

        int _controllerID = _event.jdevice.which;
        if(controllers[_controllerID]->pressedGamepadButtons[_key] == 2) return;
        controllers[_controllerID]->pressedGamepadButtons[_key] = 1;
    }

    void ControllerInput::onGamepadsButtonUp(SDL_Event& _event) {
        auto _key = static_cast<GamePadButtons>(_event.cbutton.button);
        int _controllerID = _event.cdevice.which;
        controllers[_controllerID]->pressedGamepadButtons[_key] = 0;

        JoystickButtonUpEvent _e(_key);
        window->consumeEvent(_e);
    }

    void ControllerInput::onGamepadConnected(SDL_Event& _event) {
        #if !IS_MOBILE()
        initGamepads();
        #endif
    }

    void ControllerInput::onGamepadDisconnected(SDL_Event& _event) {
        LOG_W("Removed controller: ", controllers[_event.cdevice.which]->ID)
        if(controllers[_event.cdevice.which]->ID >= 0)
            SDL_GameControllerClose(controllers[_event.cdevice.which]->sdlGameController);
        int _removedID = controllers[_event.cdevice.which]->ID;
        delete controllers[_event.cdevice.which];
        controllers.erase(_event.cdevice.which);

        for(auto& _controller : controllers)
            if(_controller.second->ID > _removedID) {
                LOG_W("Reassigned controller ", _controller.second->ID, " to ", _controller.second->ID - 1)
                _controller.second->ID--;
            }
    }

    int ControllerInput::getButtonState(int _keyOrButton, int _controllerID) {
        if(_controllerID == -1) return -1;
        return controllers[_controllerID]->pressedGamepadButtons[(GamePadButtons)_keyOrButton];
    }

    void ControllerInput::setButtonState(int _keyOrButton, int _state, int _controllerID) {
        if(_controllerID == -1) return;
        controllers[_controllerID]->pressedGamepadButtons[(GamePadButtons)_keyOrButton] = _state;
    }

    int ControllerInput::getAxisState(int _keyOrButton, int _controllerID) {
        if(_controllerID == -1) return -1;
        return controllers[_controllerID]->pressedGamepadAxis[(GamePadAxis)_keyOrButton];
    }

    void ControllerInput::setAxisState(int _keyOrButton, int _state, int _controllerID) {
        if(_controllerID == -1) return;
        controllers[_controllerID]->pressedGamepadAxis[(GamePadAxis)_keyOrButton] = _state;
    }

    Vec2F ControllerInput::getAxisValue(const GamePadAxis& _axis, int _controllerID) {
        switch (_axis) {
            case GamePadAxis::Left: return controllers[_controllerID]->leftJoystickValue;
            case GamePadAxis::Right: return controllers[_controllerID]->rightJoystickValue;
            case GamePadAxis::LT: return {controllers[_controllerID]->backButtonsValue.x, 0.f};
            case GamePadAxis::RT: return {controllers[_controllerID]->backButtonsValue.y, 0.f};
        }

        return { -1, -1 };
    }

    void ControllerInput::vibrate(const std::string& _vibrateEffect, int _controllerID) {
        SDL_HapticRunEffect( controllers.at(_controllerID)->vibration, VibrationManager::get().effects[_vibrateEffect].vibrationEffectID, 1 );
    }

    bool ControllerInput::hasController(int _id) {
        return controllers.find(playerIndexToInnerControllerID(_id)) != controllers.end();
    }

    void ControllerInput::destroy() {
        LOG_S("Cleaning up vibrations effects")
        VibrationManager::get().destroy();

        LOG_S("Cleaning up controllers")
        for(auto& _controller : controllers) {
            LOG_S("     Controller: ", _controller.second->ID)
            if(_controller.second->ID >= 0) {
                SDL_HapticClose(_controller.second->vibration);
                SDL_GameControllerClose(_controller.second->sdlGameController);
            }
            delete _controller.second;
        }
    }

    int ControllerInput::playerIndexToInnerControllerID(int _playerIndex) {
        auto _id = SDL_JoystickGetDeviceInstanceID(_playerIndex);
        if(_id == -1) return _id;
        return controllers[_id]->ID;
    }

    bool ControllerInput::reassignController(int _controllerID, int _as) {
        if(!(hasController(_controllerID) || hasController(_as) || _controllerID < 0 || _as < 0)) {
            LOG_E("Tried to reassigned controller ", _controllerID, " to ", _as, " but at least one of them didn't exist")
            return false;
        }

        controllers[SDL_JoystickGetDeviceInstanceID(_controllerID)]->ID = _as;
        controllers[SDL_JoystickGetDeviceInstanceID(_as)]->ID = _controllerID;

        return true;
    }
}