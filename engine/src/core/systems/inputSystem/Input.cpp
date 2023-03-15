#include "core/systems/inputSystem/input/Input.h"
#include "core/Engine.h"
#include "core/graph/Scene.h"
#include "core/systems/eventSystem/ControllerEvent.h"
#include "core/systems/inputSystem/input/WindowInput.h"
#include "core/systems/inputSystem/input/KeyboardInput.h"
#include "core/systems/inputSystem/input/MouseInput.h"
#include "core/systems/inputSystem/input/ControllerInput.h"
#include "core/systems/inputSystem/input/MobileInput.h"

#include "core/render/Camera.h"
#include "core/graph/components/Node.h"

#if !IS_MOBILE()
#include "imgui_impl_sdl.h"
#endif

namespace RDE {

	std::vector<RDE_MOUSE_BUTTON_> justClickedButtons;
	std::vector<RDE_MOUSE_BUTTON_> justReleasedButtons;

	std::vector<RDE_KEYBOARD_KEY_> justPressedKeys;
	std::vector<RDE_KEYBOARD_KEY_> justReleasedKeys;

	std::vector<int> justPressedFingers;
	std::vector<int> justReleasedFingers;

    bool Input::pollEvent(SDL_Event& _event) {
        if(events.find((RDE_SYSTEM_EVENT_)_event.type) == events.end()) {
            return false;
        }

        events[(int)_event.type](_event);
        return true;
    }

    bool Input::ignoreEvent(const SDL_EventType& _eventType) {
        return std::find(ignoredEvents.begin(), ignoredEvents.end(), _eventType) != ignoredEvents.end();
    }




    void InputManager::init(Engine* _engine, Window* _window) {
        engine = _engine;

        windowInput = new WindowInput;
        keyboardInput = new KeyboardInput;
        mouseInput = new MouseInput;
        controllerInput = new ControllerInput;
        mobileInput = new MobileInput;

        windowInput->init(_engine, _window);
        keyboardInput->init(_engine, _window);
        mouseInput->init(_engine, _window);
        controllerInput->init(_engine, _window);
        mobileInput->init(_engine, _window);
    }

    void InputManager::pollEvents() {
        SDL_Event _event;
        
        SDL_PumpEvents();
        while (SDL_PollEvent(&_event)) {

            bool _eventImplemented = false;

            #if !IS_MOBILE()
            ImGui_ImplSDL2_ProcessEvent(&_event);
            #endif

            _eventImplemented |= windowInput->pollEvent(_event);
            _eventImplemented |= keyboardInput->pollEvent(_event);
            _eventImplemented |= mouseInput->pollEvent(_event);
            _eventImplemented |= controllerInput->pollEvent(_event);
            _eventImplemented |= mobileInput->pollEvent(_event);

            _eventImplemented |= windowInput->ignoreEvent((SDL_EventType)_event.type);
            _eventImplemented |= keyboardInput->ignoreEvent((SDL_EventType)_event.type);
            _eventImplemented |= controllerInput->ignoreEvent((SDL_EventType)_event.type);
            _eventImplemented |= mouseInput->ignoreEvent((SDL_EventType)_event.type);
            _eventImplemented |= mobileInput->ignoreEvent((SDL_EventType)_event.type);

            if(!_eventImplemented) {
                Util::Log::warn("System event ", _event.type, " not implemented!!");
                continue;
            }
        }
    }

    bool InputManager::isKeyJustPressed(RDE_KEYBOARD_KEY_ _key) {
		if(keyboardInput->getState(_key) == RDE_INPUT_STATUS_JUST_PRESSED) {
			justPressedKeys.emplace_back(_key);
			return true;
		}

		return false;
    }

	bool InputManager::isKeyJustReleased(RDE_KEYBOARD_KEY_ _key) {
		if(keyboardInput->getState(_key) == RDE_INPUT_STATUS_JUST_RELEASED) {
			justReleasedKeys.emplace_back(_key);
			return true;
		}

		return false;
    }

	bool InputManager::isKeyPressed(RDE_KEYBOARD_KEY_ _key) {
		return keyboardInput->getState(_key) == RDE_INPUT_STATUS_KEEP_PRESSED;
    }

	bool InputManager::isKeyReleased(RDE_KEYBOARD_KEY_ _key) {
		return keyboardInput->getState(_key) == RDE_INPUT_STATUS_KEEP_RELEASED;
    }




    bool InputManager::isMouseJustPressed(RDE_MOUSE_BUTTON_ _mouseButton) {
		if(mouseInput->getState(_mouseButton) == RDE_INPUT_STATUS_JUST_PRESSED) {
			justClickedButtons.emplace_back(_mouseButton);
			return true;
		}

		return false;
    }

	bool InputManager::isMouseJustReleased(RDE_MOUSE_BUTTON_ _mouseButton) {
		if(mouseInput->getState(_mouseButton) == RDE_INPUT_STATUS_JUST_RELEASED) {
			justReleasedButtons.emplace_back(_mouseButton);
			return true;
		}

		return false;
    }

	bool InputManager::isMousePressed(RDE_MOUSE_BUTTON_ _mouseButton) {
		return mouseInput->getState(_mouseButton) == RDE_INPUT_STATUS_KEEP_PRESSED;
    }

	bool InputManager::isMouseReleased(RDE_MOUSE_BUTTON_ _mouseButton) {
		return mouseInput->getState(_mouseButton) == RDE_INPUT_STATUS_KEEP_RELEASED;
    }

    Vec2F InputManager::getMousePosScreenCoords(bool _centeredMiddleScreen) {
        auto _mousePos = mouseInput->getMousePosition();
        auto& _wi = windowInput;
        float _x = _mousePos.x - (_centeredMiddleScreen ? (float)_wi->window->getWindowSize().x / 2.f : 0.f);
        float _y = (_centeredMiddleScreen ? (float)_wi->window->getWindowSize().y / 2.f : 0.f) - _mousePos.y;
        float _zoom = mouseInput->engine->manager.sceneManager.getDisplayedScene()->mainCamera->getCurrentZoomLevel();
        return {_x * _zoom, _y * _zoom};
    }

    Vec2F InputManager::getMousePosWorldPos() {
        auto _mousePos = mouseInput->getMousePosition();
        auto& _wi = windowInput;
        auto& _camera = *mouseInput->engine->manager.sceneManager.getDisplayedScene()->mainCamera;
        float _x = _mousePos.x - (float)_wi->window->getWindowSize().x / 2.f + _camera.node->getTransform()->getPosition().x;
        float _y = _mousePos.y - (float)_wi->window->getWindowSize().y / 2.f + _camera.node->getTransform()->getPosition().y;
        float _zoom = _camera.getCurrentZoomLevel();
        return {_x * _zoom, - _y * _zoom};
    }

    Vec2F InputManager::getMousePosCanvas() {
        auto _mousePos = mouseInput->getMousePosition();
        auto& _wi = windowInput;
        auto& _camera = *mouseInput->engine->manager.sceneManager.getDisplayedScene()->mainCamera;
        float _x = _mousePos.x - (float)_wi->window->getWindowSize().x / 2.f + _camera.node->getTransform()->getPosition().x;
        float _y = _mousePos.y - (float)_wi->window->getWindowSize().y / 2.f + _camera.node->getTransform()->getPosition().y;
        return {_x, - _y};
    }




    bool InputManager::reassignController(int _controllerID, int _as) {
        return controllerInput->reassignController(_controllerID, _as);
    }

    bool InputManager::isGamepadButtonJustPressed(RDE_CONTROLLER_BUTTON_ _button, int _controllerID) {
        auto* _controllerInput = controllerInput;
        if(!_controllerInput->hasController(_controllerID)) return false;

        if(_controllerInput->getButtonState((int)_button, _controllerInput->playerIndexToInnerControllerID(_controllerID)) == 1) {
            _controllerInput->setButtonState((int)_button, 2, _controllerInput->playerIndexToInnerControllerID(_controllerID));
            return true;
        }

        return false;
    }

	bool InputManager::isGamepadButtonJustReleased(RDE_CONTROLLER_BUTTON_ _button, int _controllerID) {
        auto* _controllerInput = controllerInput;
        if(!_controllerInput->hasController(_controllerID)) {
            Util::Log::warn("IS FALSE FOR ", _controllerID);
            return false;
        }

        if(_controllerInput->getButtonState((int)_button, _controllerInput->playerIndexToInnerControllerID(_controllerID)) == 0) {
            _controllerInput->setButtonState((int)_button, 3, _controllerInput->playerIndexToInnerControllerID(_controllerID));
            return true;
        }

        return false;
    }

	bool InputManager::isGamepadButtonPressed(RDE_CONTROLLER_BUTTON_ _button, int _controllerID) {
        auto* _controllerInput = controllerInput;
        if(!_controllerInput->hasController(_controllerID)) return false;
        return _controllerInput->getButtonState((int)_button, _controllerInput->playerIndexToInnerControllerID(_controllerID)) == 1;
    }

	bool InputManager::isGamepadButtonReleased(RDE_CONTROLLER_BUTTON_ _button, int _controllerID) {
        auto* _controllerInput = controllerInput;
        if(!_controllerInput->hasController(_controllerID)) return false;
        return _controllerInput->getButtonState((int)_button, _controllerInput->playerIndexToInnerControllerID(_controllerID)) == 0;
    }

    bool InputManager::isGamepadAxisPressed(RDE_CONTROLLER_AXIS_ _axis, int _controllerID) {
        auto* _controllerInput = controllerInput;
        if(!_controllerInput->hasController(_controllerID)) return false;
        return _controllerInput->getAxisState((int)_axis, _controllerInput->playerIndexToInnerControllerID(_controllerID)) == 0;
    }

	bool InputManager::isGamepadAxisJustPressed(RDE_CONTROLLER_AXIS_ _axis, int _controllerID) {
        auto* _controllerInput = controllerInput;
        if(!_controllerInput->hasController(_controllerID)) return false;
        if(_controllerInput->getAxisState((int)_axis, _controllerInput->playerIndexToInnerControllerID(_controllerID)) == 1) {
            _controllerInput->setAxisState((int)_axis, 2, _controllerInput->playerIndexToInnerControllerID(_controllerID));
            return true;
        }

        return false;
    }

	bool InputManager::isGamepadAxisReleased(RDE_CONTROLLER_AXIS_ _axis, int _controllerID) {
        auto* _controllerInput = controllerInput;
        if(!_controllerInput->hasController(_controllerID)) return false;
        return _controllerInput->getAxisState((int)_axis, _controllerInput->playerIndexToInnerControllerID(_controllerID)) == 0;
    }

	bool InputManager::gamepadVibrate(int _controllerID, const std::string& _vibrationEffectName) {
		auto* _controllerInput = controllerInput;
		if(!_controllerInput->hasController(_controllerID)) return false;
		_controllerInput->vibrate(_vibrationEffectName, _controllerID);
		return true;
	}





    bool InputManager::isMobileScreenJustPressed(int _fingerID) {
		if(mobileInput->getState(_fingerID) == RDE_INPUT_STATUS_JUST_PRESSED) {
			justPressedFingers.emplace_back(_fingerID);
			return true;
		}

		return false;
    }

    bool InputManager::isMobileScreenJustReleased(int _fingerID) {
		if(mobileInput->getState(_fingerID) == RDE_INPUT_STATUS_JUST_RELEASED) {
			justReleasedFingers.emplace_back(_fingerID);
			return true;
		}

		return false;
    }

    bool InputManager::isMobileScreenPressed(int _fingerID) {
		return mobileInput->getState(_fingerID) == RDE_INPUT_STATUS_KEEP_PRESSED;
    }

    bool InputManager::isMobileScreenRelease(int _fingerID) {
		return mobileInput->getState(_fingerID) == RDE_INPUT_STATUS_KEEP_RELEASED;
    }

    std::vector<RDE_SYSTEM_EVENT_> InputManager::getEventsIgnored(const RDE_INPUT_TYPE_& _inputType) {
        switch (_inputType) {
			case RDE_INPUT_TYPE_WINDOW: return windowInput->ignoredEvents;
			case RDE_INPUT_TYPE_MOUSE: return mouseInput->ignoredEvents;
			case RDE_INPUT_TYPE_KEYBOARD: return keyboardInput->ignoredEvents;
			case RDE_INPUT_TYPE_CONTROLLER: return controllerInput->ignoredEvents;
			case RDE_INPUT_TYPE_MOBILE: return mobileInput->ignoredEvents;
        }

        return {  };
    }

	void InputManager::addEventToIgnore(const RDE_INPUT_TYPE_& _inputType, const RDE_SYSTEM_EVENT_& _event) {
        switch (_inputType) {
			case RDE_INPUT_TYPE_WINDOW: {
                auto& _v = windowInput->ignoredEvents;
                if(std::find(_v.begin(), _v.end(), _event) == _v.end())
                    windowInput->ignoredEvents.push_back(_event);
            }
			case RDE_INPUT_TYPE_MOUSE: {
                auto& _v = mouseInput->ignoredEvents;
                if(std::find(_v.begin(), _v.end(), _event) == _v.end())
                mouseInput->ignoredEvents.push_back(_event);
            }
			case RDE_INPUT_TYPE_KEYBOARD: {
                auto& _v = keyboardInput->ignoredEvents;
                if(std::find(_v.begin(), _v.end(), _event) == _v.end())
                    keyboardInput->ignoredEvents.push_back(_event);
            }
			case RDE_INPUT_TYPE_CONTROLLER: {
                auto& _v = controllerInput->ignoredEvents;
                if(std::find(_v.begin(), _v.end(), _event) == _v.end())
                    controllerInput->ignoredEvents.push_back(_event);
            }
			case RDE_INPUT_TYPE_MOBILE: {
                auto& _v = mobileInput->ignoredEvents;
                if(std::find(_v.begin(), _v.end(), _event) == _v.end())
                    mobileInput->ignoredEvents.push_back(_event);
            }
        }
    }

    void InputManager::removeEventToIgnore(const RDE_INPUT_TYPE_& _inputType, const RDE_SYSTEM_EVENT_& _event) {
        switch (_inputType) {
			case RDE_INPUT_TYPE_WINDOW: {
                auto& _v = windowInput->ignoredEvents;
                _v.erase(std::remove(_v.begin(), _v.end(), _event), _v.end());
            }
			case RDE_INPUT_TYPE_MOUSE: {
                auto& _v = mouseInput->ignoredEvents;
                _v.erase(std::remove(_v.begin(), _v.end(), _event), _v.end());
            }
			case RDE_INPUT_TYPE_KEYBOARD: {
                auto& _v = keyboardInput->ignoredEvents;
                _v.erase(std::remove(_v.begin(), _v.end(), _event), _v.end());
            }
			case RDE_INPUT_TYPE_CONTROLLER: {
                auto& _v = controllerInput->ignoredEvents;
                _v.erase(std::remove(_v.begin(), _v.end(), _event), _v.end());
            }
			case RDE_INPUT_TYPE_MOBILE: {
                auto& _v = mobileInput->ignoredEvents;
                _v.erase(std::remove(_v.begin(), _v.end(), _event), _v.end());
            }
        }
    }

	void InputManager::syncEvents() {
		for(auto _i = 0; _i < justClickedButtons.size(); _i++) {
			mouseInput->setState(justClickedButtons[_i], RDE_INPUT_STATUS_KEEP_PRESSED);
		}
		justClickedButtons.clear();

		for(auto _i = 0; _i < justReleasedButtons.size(); _i++) {
			mouseInput->setState(justReleasedButtons[_i], RDE_INPUT_STATUS_KEEP_RELEASED);
		}
		justReleasedButtons.clear();



		for(auto _i = 0; _i < justPressedKeys.size(); _i++) {
			keyboardInput->setState(justPressedKeys[_i], RDE_INPUT_STATUS_KEEP_PRESSED);
		}
		justPressedKeys.clear();

		for(auto _i = 0; _i < justReleasedKeys.size(); _i++) {
			keyboardInput->setState(justReleasedKeys[_i], RDE_INPUT_STATUS_KEEP_RELEASED);
		}
		justReleasedKeys.clear();



		for(auto _i = 0; _i < justPressedFingers.size(); _i++) {
			mobileInput->setState(justPressedFingers[_i], RDE_INPUT_STATUS_KEEP_PRESSED);
		}
		justPressedFingers.clear();

		for(auto _i = 0; _i < justReleasedFingers.size(); _i++) {
			mobileInput->setState(justReleasedFingers[_i], RDE_INPUT_STATUS_KEEP_RELEASED);
		}
		justReleasedFingers.clear();
	}

    void InputManager::destroy() {
        delete windowInput;
        delete mouseInput;
        delete keyboardInput;

        controllerInput->destroy();
        delete controllerInput;
        delete mobileInput;
    }
}
