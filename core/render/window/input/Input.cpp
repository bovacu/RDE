#include "Input.h"
#include "core/Engine.h"
#include "core/render/window/event/JoystickEvent.h"

#include <utility>

namespace engine {

    InputManager& InputManager::get() {
        static InputManager _inputManager;
        return _inputManager;
    }

    void InputManager::init(Window* _window) {
        windowInput.init(_window);
        keyboardInput.init(_window);
        mouseInput.init(_window);
        controllerInput.init(_window);
    }

    void InputManager::pollEvents() {
        SDL_Event _event;
        while (SDL_PollEvent(&_event)) {

            bool _eventImplemented = false;

            ImGui_ImplSDL2_ProcessEvent(&_event);

            _eventImplemented |= windowInput.pollEvent(_event);
            _eventImplemented |= keyboardInput.pollEvent(_event);
            _eventImplemented |= mouseInput.pollEvent(_event);
            _eventImplemented |= controllerInput.pollEvent(_event);

            if(!_eventImplemented) {
                LOG_W("System event ", _event.type, " not implemented!!")
                continue;
            }
        }
    }

    void InputManager::setEventCallback(std::function<void(Event&)> _eventCallback) {
        eventCallback = std::move(_eventCallback);
    }

    bool InputManager::isKeyJustPressed(KeyCode _key) {
        if(get().keyboardInput.getState((int)_key) == 1) {
            get().keyboardInput.setState((int)_key, 2);
            return true;
        }

        return false;
    }

    bool InputManager::isKeyPressed(KeyCode _key) {
        return get().keyboardInput.getState((int)_key) == 1;
    }

    bool InputManager::isKeyReleased(KeyCode _key) {
        return get().keyboardInput.getState((int)_key) == 0;
    }



    bool InputManager::isMouseJustPressed(MouseCode _mouseButton) {
        if(get().mouseInput.getState((int)_mouseButton) == 1) {
            get().mouseInput.setState((int)_mouseButton, 2);
            return true;
        }

        return false;
    }

    bool InputManager::isMousePressed(MouseCode _button) {
        return get().mouseInput.getState((int)_button) == 1;
    }

    bool InputManager::isMouseReleased(MouseCode _button) {
        return  get().mouseInput.getState((int)_button) == 0;
    }

    Vec2F InputManager::getMousePosition() {
        return get().mouseInput.getMousePosition();
    }

    int InputManager::getMouseX() {
        return getMousePosition().x;
    }

    int InputManager::getMouseY() {
        return getMousePosition().y;
    }

    bool InputManager::isGamepadButtonJustPressed(GamePadKeys _button) {
//        if(inputSystem->pressedGamepadKeys[_button] == 1) {
//            inputSystem->pressedGamepadKeys[_button] = 2;
//            return true;
//        }

        return false;
    }

    bool InputManager::isGamepadButtonPressed(GamePadKeys _button) {
//        return inputSystem->pressedGamepadKeys[_button] == 1;
        return false;
    }

    bool InputManager::isGamepadButtonReleased(GamePadKeys _button) {
//        return inputSystem->pressedGamepadKeys[_button] == 0;
        return false;
    }

    bool InputManager::isGamepadAxisPressed(GamePadAxis _axis) {
//        switch (_axis) {
//            case GamePadAxis::Left: return inputSystem->leftJoystickValue.x != 0 || inputSystem->leftJoystickValue.y != 0;
//            case GamePadAxis::Right: return inputSystem->rightJoystickValue.x != 0 || inputSystem->rightJoystickValue.y != 0;
//            case GamePadAxis::LT: return inputSystem->backButtonsValue.x != 0;
//            case GamePadAxis::RT: return inputSystem->backButtonsValue.y != 0;
//        }

        return false;
    }

    bool InputManager::isGamepadAxisJustPressed(GamePadAxis _axis) {
        return false;
    }

    bool InputManager::isGamepadAxisReleased(GamePadAxis _axis) {
        return false;
    }
}