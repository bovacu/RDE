// Created by borja on 6/2/22.


#include "core/systems/inputSystem/input/ControllerInput.h"
#include "core/systems/eventSystem/ControllerEvent.h"
#include "core/Engine.h"

namespace RDE {

    void ControllerVibrationManager::init(std::unordered_map<int, Controller*>* _controllers) {
        controllers = _controllers;
        addVibrationEffect("default", {
            500,
            0,
            0,
            15000,
            1000
        });
    }

    void ControllerVibrationManager::addVibrationEffect(const std::string& _effectName, const VibrationConfig& _config) {
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

    void ControllerVibrationManager::removeVibrationEffect(const std::string& _effectName) {
        for(auto _controllerID : effects[_effectName].controllerIDs)
            SDL_HapticDestroyEffect( controllers->at(_controllerID)->vibration, effects[_effectName].vibrationEffectID);
        effects.erase(_effectName);
    }

    void ControllerVibrationManager::assignVibrationEffectToController(const std::string& _effectName, int _controllerID) {
        effects[_effectName].vibrationEffectID = SDL_HapticNewEffect(controllers->at(_controllerID)->vibration, &effects[_effectName].vibrationEffect);
        effects[_effectName].controllerIDs.emplace_back(_controllerID);
    }

    void ControllerVibrationManager::destroy() {
        for(auto& _effect : effects) {
            for(auto _controllerID : _effect.second.controllerIDs)
                SDL_HapticDestroyEffect( controllers->at(_controllerID)->vibration, _effect.second.vibrationEffectID);
        }
    }


    Controller::Controller(int _controllerID) : ID(_controllerID) {

        pressedGamepadButtons = {
                {RDE_CONTROLLER_BUTTON_A,       	-1},
                {RDE_CONTROLLER_BUTTON_B,       	-1},
                {RDE_CONTROLLER_BUTTON_X,       	-1},
                {RDE_CONTROLLER_BUTTON_Y,       	-1},
                {RDE_CONTROLLER_BUTTON_DPAD_DOWN,   -1},
                {RDE_CONTROLLER_BUTTON_DPAD_LEFT,   -1},
                {RDE_CONTROLLER_BUTTON_DPAD_RIGHT,  -1},
                {RDE_CONTROLLER_BUTTON_DPAD_UP,     -1},
                {RDE_CONTROLLER_BUTTON_GUIDE,       -1},
                {RDE_CONTROLLER_BUTTON_LB,          -1},
                {RDE_CONTROLLER_BUTTON_RB,          -1},
                {RDE_CONTROLLER_BUTTON_L_JOY,  		-1},
                {RDE_CONTROLLER_BUTTON_R_JOY, 		-1},
                {RDE_CONTROLLER_BUTTON_SELECT,      -1},
                {RDE_CONTROLLER_BUTTON_START,       -1},
        };

        pressedGamepadAxis = {
                {RDE_CONTROLLER_AXIS_RIGHT, -1},
                {RDE_CONTROLLER_AXIS_LEFT,  -1},
                {RDE_CONTROLLER_AXIS_RT,    -1},
                {RDE_CONTROLLER_AXIS_LT,    -1},
        };
    }

    void ControllerInput::init(Engine* _engine, Window* _window) {
        engine = _engine;
        window = _window;

        UniqueDelegate<void(SDL_Event&)> _gpJ, _gpBD, _gpBU, _gpD, _gpC;
        _gpJ.bind<&ControllerInput::onControllerMoved>(this);
        _gpBD.bind<&ControllerInput::onControllerButtonDown>(this);
        _gpBU.bind<&ControllerInput::onControllerButtonUp>(this);
        _gpC.bind<&ControllerInput::onControllerConnected>(this);
        _gpD.bind<&ControllerInput::onControllerDisconnected>(this);

        events[RDE_SYSTEM_EVENT_GAMEPAD_JOYSTICK] = _gpJ;
        events[RDE_SYSTEM_EVENT_GAMEPAD_BUTTON_DOWN] = _gpBD;
        events[RDE_SYSTEM_EVENT_GAMEPAD_BUTTON_UP] = _gpBU;
        events[RDE_SYSTEM_EVENT_GAMEPAD_CONNECTED_E] = _gpC;
        events[RDE_SYSTEM_EVENT_GAMEPAD_DISCONNECTED_E] = _gpD;

		ignoredEvents = { RDE_SYSTEM_EVENT_JOYSTICK_HAT_MOTION_E, RDE_SYSTEM_EVENT_JOYSTICK_BALL_MOTION_E, RDE_SYSTEM_EVENT_JOYSTICK_BUTTON_DOWN_E, RDE_SYSTEM_EVENT_JOYSTICK_BUTTON_UP_E, RDE_SYSTEM_EVENT_JOYSTICK_CONNECTED_E,
			RDE_SYSTEM_EVENT_JOYSTICK_DISCONNECTED_E, RDE_SYSTEM_EVENT_JOYSTICK_AXIS_MOTION_E };

        engine->manager.controllerVibrationManager.init(&controllers);
    }

    void ControllerInput::initControllers() {
        int _numJoysticks = SDL_NumJoysticks();
        Util::Log::debug("Num of Joysticks: ", _numJoysticks);
        if(_numJoysticks < 1) {
            Util::Log::debug("No joysticks connected");
            return;
        }
        for(int _i = 0; _i < _numJoysticks; _i++) {
            auto* _currentGameController = SDL_GameControllerOpen(_i);
            if(_currentGameController == nullptr) {
                Util::Log::error("Error with controller ", _i);
                auto _controller = new Controller(-100);
                _controller->sdlGameController = _currentGameController;
                controllers[SDL_JoystickGetDeviceInstanceID(_i)] = _controller;
                continue;
            }

            if(controllers[SDL_JoystickGetDeviceInstanceID(_i)] != nullptr) continue;

            Util::Log::debug("Connected Controller ", _i, ", Device Instance: ", SDL_JoystickGetDeviceInstanceID(_i));
            auto _controller = new Controller(_i);
            _controller->sdlGameController = _currentGameController;
            _controller->vibration = SDL_HapticOpenFromJoystick(SDL_GameControllerGetJoystick(_currentGameController));
            controllers[SDL_JoystickGetDeviceInstanceID(_i)] = _controller;

            engine->manager.controllerVibrationManager.assignVibrationEffectToController("default", _controller->ID);
        }

        Util::Log::debug(_numJoysticks, " connected and ", controllers.size(), " loaded");
    }

    void ControllerInput::onControllerMoved(SDL_Event& _event) {
        Vec2F _left;
        if(_event.caxis.axis == 0) _left.x = (float)_event.caxis.value / (float)SDL_JOYSTICK_AXIS_MAX;
        if(_event.caxis.axis == 1) _left.y = (float)_event.caxis.value / (float)SDL_JOYSTICK_AXIS_MAX;

        Vec2F _right;
        if(_event.caxis.axis == 2) _right.x = (float)_event.caxis.value / (float)SDL_JOYSTICK_AXIS_MAX;
        if(_event.caxis.axis == 3) _right.y = (float)_event.caxis.value / (float)SDL_JOYSTICK_AXIS_MAX;

        Vec2F _back;
        if(_event.caxis.axis == 4) _back.x = (float)_event.caxis.value / (float)SDL_JOYSTICK_AXIS_MAX;
        if(_event.caxis.axis == 5) _back.y = (float)_event.caxis.value / (float)SDL_JOYSTICK_AXIS_MAX;

        ControllerAxisMovedEvent _e(_left, _right, _back);
        window->consumeEvent(_e);

        int _controllerID = _event.cdevice.which;

        controllers[_controllerID]->leftJoystickValue = _left;
        controllers[_controllerID]->rightJoystickValue = _right;
        controllers[_controllerID]->backButtonsValue = _back;

        float _epsilon = 0.001f;

        if(std::abs(_left.x) <= _epsilon && std::abs(_left.y) <= _epsilon)
            controllers[_controllerID]->pressedGamepadAxis[RDE_CONTROLLER_AXIS_LEFT] = 0;
		else if(controllers[_controllerID]->pressedGamepadAxis[RDE_CONTROLLER_AXIS_LEFT] != 2)
			controllers[_controllerID]->pressedGamepadAxis[RDE_CONTROLLER_AXIS_LEFT] = 1;

        if(std::abs(_right.x) <= _epsilon && std::abs(_right.y) <= _epsilon)
			controllers[_controllerID]->pressedGamepadAxis[RDE_CONTROLLER_AXIS_RIGHT] = 0;
		else if(controllers[_controllerID]->pressedGamepadAxis[RDE_CONTROLLER_AXIS_RIGHT] != 2)
			controllers[_controllerID]->pressedGamepadAxis[RDE_CONTROLLER_AXIS_RIGHT] = 1;

        if(std::abs(_back.x) <= _epsilon)
			controllers[_controllerID]->pressedGamepadAxis[RDE_CONTROLLER_AXIS_LT] = 0;
		else if(controllers[_controllerID]->pressedGamepadAxis[RDE_CONTROLLER_AXIS_LT] != 2)
			controllers[_controllerID]->pressedGamepadAxis[RDE_CONTROLLER_AXIS_LT] = 1;

        if(std::abs(_back.y) <= _epsilon)
			controllers[_controllerID]->pressedGamepadAxis[RDE_CONTROLLER_AXIS_RT] = 0;
		else if(controllers[_controllerID]->pressedGamepadAxis[RDE_CONTROLLER_AXIS_RT] != 2)
			controllers[_controllerID]->pressedGamepadAxis[RDE_CONTROLLER_AXIS_RT] = 1;
    }

    void ControllerInput::onControllerButtonDown(SDL_Event& _event) {
        auto _key = static_cast<RDE_CONTROLLER_BUTTON_>(_event.cbutton.button);

        ControllerButtonDownEvent _e(_key);
        window->consumeEvent(_e);

        int _controllerID = _event.jdevice.which;
        if(controllers[_controllerID]->pressedGamepadButtons[_key] == 2) return;
        controllers[_controllerID]->pressedGamepadButtons[_key] = 1;
    }

    void ControllerInput::onControllerButtonUp(SDL_Event& _event) {
		auto _key = static_cast<RDE_CONTROLLER_BUTTON_>(_event.cbutton.button);
        int _controllerID = _event.cdevice.which;
        controllers[_controllerID]->pressedGamepadButtons[_key] = 0;

        ControllerButtonUpEvent _e(_key);
        window->consumeEvent(_e);
    }

    void ControllerInput::onControllerConnected(SDL_Event& _event) {
        #if !IS_MOBILE()
        initControllers();
        #endif
    }

    void ControllerInput::onControllerDisconnected(SDL_Event& _event) {
        Util::Log::debug("Removed controller: ", controllers[_event.cdevice.which]->ID);
        if(controllers[_event.cdevice.which]->ID >= 0)
            SDL_GameControllerClose(controllers[_event.cdevice.which]->sdlGameController);
        int _removedID = controllers[_event.cdevice.which]->ID;
        delete controllers[_event.cdevice.which];
        controllers.erase(_event.cdevice.which);

        for(auto& _controller : controllers)
            if(_controller.second->ID > _removedID) {
                Util::Log::debug("Reassigned controller ", _controller.second->ID, " to ", _controller.second->ID - 1);
                _controller.second->ID--;
            }
    }

    int ControllerInput::getButtonState(int _keyOrButton, int _controllerID) {
        if(_controllerID == -1) return -1;
		return controllers[_controllerID]->pressedGamepadButtons[(RDE_CONTROLLER_BUTTON_)_keyOrButton];
    }

    void ControllerInput::setButtonState(int _keyOrButton, int _state, int _controllerID) {
        if(_controllerID == -1) return;
		controllers[_controllerID]->pressedGamepadButtons[(RDE_CONTROLLER_BUTTON_)_keyOrButton] = _state;
    }

    int ControllerInput::getAxisState(int _keyOrButton, int _controllerID) {
        if(_controllerID == -1) return -1;
		return controllers[_controllerID]->pressedGamepadAxis[(RDE_CONTROLLER_AXIS_)_keyOrButton];
    }

    void ControllerInput::setAxisState(int _keyOrButton, int _state, int _controllerID) {
        if(_controllerID == -1) return;
		controllers[_controllerID]->pressedGamepadAxis[(RDE_CONTROLLER_AXIS_)_keyOrButton] = _state;
    }

	Vec2F ControllerInput::getAxisValue(const RDE_CONTROLLER_AXIS_& _axis, int _controllerID) {
        switch (_axis) {
			case RDE_CONTROLLER_AXIS_LEFT: return controllers[_controllerID]->leftJoystickValue;
			case RDE_CONTROLLER_AXIS_RIGHT: return controllers[_controllerID]->rightJoystickValue;
			case RDE_CONTROLLER_AXIS_LT: return {controllers[_controllerID]->backButtonsValue.x, 0.f};
			case RDE_CONTROLLER_AXIS_RT: return {controllers[_controllerID]->backButtonsValue.y, 0.f};
        }

        return { -1, -1 };
    }

    void ControllerInput::vibrate(const std::string& _vibrateEffect, int _controllerID) {
        SDL_HapticRunEffect(controllers.at(_controllerID)->vibration, engine->manager.controllerVibrationManager.effects[_vibrateEffect].vibrationEffectID, 1 );
    }

    bool ControllerInput::hasController(int _id) {
        return controllers.find(playerIndexToInnerControllerID(_id)) != controllers.end();
    }

    void ControllerInput::destroy() {
        engine->manager.controllerVibrationManager.destroy();

        Util::Log::debug("Cleaning up Controllers");
        for(auto& _controller : controllers) {
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
            Util::Log::error("Tried to reassigned controller ", _controllerID, " to ", _as, " but at least one of them didn't exist");
            return false;
        }

        controllers[SDL_JoystickGetDeviceInstanceID(_controllerID)]->ID = _as;
        controllers[SDL_JoystickGetDeviceInstanceID(_as)]->ID = _controllerID;

        return true;
    }
}