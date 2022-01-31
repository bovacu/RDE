#include "Input.h"
#include "core/Engine.h"
#include "core/render/window/event/JoystickEvent.h"

#include <utility>

namespace engine {

    InputSystem& InputSystem::get() {
        static InputSystem _input;
        return _input;
    }

    void InputSystem::init(Window* _window) {
        window = _window;

        events[SystemEventEnum::WINDOW_EVENT] = BIND_FUNC_1(InputSystem::onWindowEvent);
        events[SystemEventEnum::KEY_DOWN_E] = BIND_FUNC_1(InputSystem::onKeyDown);
        events[SystemEventEnum::KEY_UP_E] = BIND_FUNC_1(InputSystem::onKeyUp);
        events[SystemEventEnum::QUIT_E] = BIND_FUNC_1(InputSystem::onQuit);
        events[SystemEventEnum::MOUSE_MOVED_E] = BIND_FUNC_1(InputSystem::onMouseMoved);
        events[SystemEventEnum::MOUSE_DOWN_E] = BIND_FUNC_1(InputSystem::onMouseDown);
        events[SystemEventEnum::MOUSE_UP_E] = BIND_FUNC_1(InputSystem::onMouseUp);
        events[SystemEventEnum::MOUSE_SCROLLED_E] = BIND_FUNC_1(InputSystem::onMouseScroll);
        events[SystemEventEnum::INPUT_TEXT_E] = [](SDL_Event&) {  };
        events[SystemEventEnum::FOO_1] = [](SDL_Event&) {  }; // 772
        events[SystemEventEnum::FOO_3] = [](SDL_Event&) {  }; // 4352
        events[SystemEventEnum::FOO_2] = [](SDL_Event&) {  };
        events[SystemEventEnum::GAMEPAD_JOYSTICK] = BIND_FUNC_1(InputSystem::onGamepadsMoved);
        events[SystemEventEnum::GAMEPAD_BUTTON_DOWN] = BIND_FUNC_1(InputSystem::onGamepadsButtonDown);
        events[SystemEventEnum::GAMEPAD_BUTTON_UP] = BIND_FUNC_1(InputSystem::onGamepadsButtonUp);

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

        pressedKeyboardKeys = {
                {KeyCode::LeftControl,  0},
//            {KeyCode::LeftSuper,    0},
                {KeyCode::LeftAlt, 0},
                {KeyCode::Space,        0},
                {KeyCode::RightAlt,     0},
                {KeyCode::RightControl, 0},
                {KeyCode::LeftShift,    0},
                {KeyCode::Z,            0},
                {KeyCode::X,            0},
                {KeyCode::C,            0},
                {KeyCode::V,            0},
                {KeyCode::B,            0},
                {KeyCode::N,            0},
                {KeyCode::M,            0},
                {KeyCode::Comma,        0},
                {KeyCode::Period,       0},
                {KeyCode::Slash,        0},
                {KeyCode::RightShift,   0},
                {KeyCode::CapsLock,     0},
                {KeyCode::A,            0},
                {KeyCode::S,            0},
                {KeyCode::D,            0},
                {KeyCode::F,            0},
                {KeyCode::G,            0},
                {KeyCode::H,            0},
                {KeyCode::J,            0},
                {KeyCode::K,            0},
                {KeyCode::L,            0},
                {KeyCode::Semicolon,    0},
                {KeyCode::Apostrophe,   0},
                {KeyCode::Enter,        0},
                {KeyCode::Tab,          0},
                {KeyCode::Q,            0},
                {KeyCode::W,            0},
                {KeyCode::E,            0},
                {KeyCode::R,            0},
                {KeyCode::T,            0},
                {KeyCode::Y,            0},
                {KeyCode::U,            0},
                {KeyCode::I,            0},
                {KeyCode::O,            0},
                {KeyCode::P,            0},
                {KeyCode::LeftBracket,  0},
                {KeyCode::RightBracket, 0},
//            {KeyCode::RightSuper,   0},
                {KeyCode::Backslash,    0},
                {KeyCode::GraveAccent,  0},
                {KeyCode::D0,           0},
                {KeyCode::D1,           0},
                {KeyCode::D2,           0},
                {KeyCode::D3,           0},
                {KeyCode::D4,           0},
                {KeyCode::D5,           0},
                {KeyCode::D6,           0},
                {KeyCode::D7,           0},
                {KeyCode::D8,           0},
                {KeyCode::D9,           0},
                {KeyCode::Minus,        0},
                {KeyCode::Equal,        0},
                {KeyCode::Backspace,    0},
                {KeyCode::Escape,       0},
                {KeyCode::F1,           0},
                {KeyCode::F2,           0},
                {KeyCode::F3,           0},
                {KeyCode::F4,           0},
                {KeyCode::F5,           0},
                {KeyCode::F6,           0},
                {KeyCode::F7,           0},
                {KeyCode::F8,           0},
                {KeyCode::F9,           0},
                {KeyCode::F10,          0},
                {KeyCode::F11,          0},
                {KeyCode::F12,          0},
                {KeyCode::PrintScreen,  0},
                {KeyCode::ScrollLock,   0},
                {KeyCode::Insert,       0},
                {KeyCode::Delete,       0},
                {KeyCode::Home,         0},
                {KeyCode::End,          0},
                {KeyCode::PageUp,       0},
                {KeyCode::PageDown,     0},
                {KeyCode::Right,        0},
                {KeyCode::Left,         0},
                {KeyCode::Up,           0},
                {KeyCode::Down,         0},

        };

        initGamepads();
    }

    void InputSystem::pollEvents() {
        SDL_Event _event;
        while (SDL_PollEvent(&_event)) {

            ImGui_ImplSDL2_ProcessEvent(&_event);

            if(events.find((SystemEventEnum)_event.type) == events.end()) {
                LOG_W("System event ", _event.type, " not implemented!!")
                continue;
            }

            events[(SystemEventEnum)_event.type](_event);

        }
    }

    void InputSystem::setEventCallback(std::function<void(Event&)> _eventCallback) {
        eventCallback = std::move(_eventCallback);
    }

    void InputSystem::onKeyDown(SDL_Event& _event) {
        auto _key = static_cast<KeyCode>(_event.key.keysym.scancode);
        if(pressedKeyboardKeys[_key] == 2) return;
        pressedKeyboardKeys[_key] = 1;
    }

    void InputSystem::onKeyUp(SDL_Event& _event) {
        auto _key = static_cast<KeyCode>(_event.key.keysym.scancode);
        pressedKeyboardKeys[_key] = 0;
    }

    void InputSystem::onMouseMoved(SDL_Event& _event) {
        mousePos = {_event.motion.x, _event.motion.y};
    }

    void InputSystem::onMouseDown(SDL_Event& _event) {
        auto _key = static_cast<MouseCode>(_event.button.button);
        if(pressedMouseButtons[_key] == 2) return;
        pressedMouseButtons[_key] = 1;
    }

    void InputSystem::onMouseUp(SDL_Event& _event) {
        auto _key = static_cast<MouseCode>(_event.button.button);
        pressedMouseButtons[_key] = 0;
    }

    void InputSystem::onMouseScroll(SDL_Event& _event) {
        MouseScrolledEvent _e(_event.wheel.preciseX, _event.wheel.preciseY);
        window->consumeEvent(_e);
    }


    // ----------------------------------------------------------------------


    void InputSystem::onWindowEvent(SDL_Event& _event) {
        switch (_event.window.event) {
            case SDL_WINDOWEVENT_RESIZED: onWindowResize(_event); break;
            case SDL_WINDOWEVENT_FOCUS_GAINED: onWindowGainFocus(_event); break;
            case SDL_WINDOWEVENT_FOCUS_LOST: onWindowLostFocus(_event); break;
            case SDL_WINDOWEVENT_ENTER: onWindowEnter(_event); break;
            case SDL_WINDOWEVENT_LEAVE: onWindowExit(_event); break;
            case SDL_WINDOWEVENT_MOVED: onWindowMoved(_event); break;
            case SDL_WINDOWEVENT_MINIMIZED: onWindowMinimized(_event); break;
            case SDL_WINDOWEVENT_MAXIMIZED: onWindowMaximized(_event); break;
            case SDL_WINDOWEVENT_CLOSE: onQuit(_event); break;
        }
    }

    void InputSystem::onWindowResize(SDL_Event& _event) {
        window->setWindowSize(_event.window.data1, _event.window.data2);
        WindowResizedEvent _rwEvent(_event.window.data1, _event.window.data2);
        window->consumeEvent(_rwEvent);
    }

    void InputSystem::onQuit(SDL_Event& _event) {
        Engine::get().setRunning(false);
    }

    // TODO implement onWindowEnter and create its associated event
    void InputSystem::onWindowEnter(SDL_Event& _event) {

    }

    // TODO implement onWindowExit and create its associated event
    void InputSystem::onWindowExit(SDL_Event& _event) {

    }

    // TODO implement onWindowGainFocus and create its associated event
    void InputSystem::onWindowGainFocus(SDL_Event& _event) {

    }

    // TODO implement onWindowLostFocus and create its associated event
    void InputSystem::onWindowLostFocus(SDL_Event& _event) {

    }

    void InputSystem::onWindowMoved(SDL_Event& _event) {
        WindowMovedEvent _wmEvent(_event.window.data1, _event.window.data2);
        window->setPosition({_event.window.data1, _event.window.data2});
        window->consumeEvent(_wmEvent);
    }

    void InputSystem::onWindowMinimized(SDL_Event& _event) {
        WindowMinimizedEvent _e(1);
        window->consumeEvent(_e);
    }

    void InputSystem::onWindowMaximized(SDL_Event& _event) {
        WindowMinimizedEvent _e(0);
        window->consumeEvent(_e);
    }


    // -----------------------------------------------


    void InputSystem::initGamepads() {
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

    void InputSystem::onGamepadsMoved(SDL_Event& _event) {
        Vec2F _left;
        if(_event.caxis.axis == 0) _left.x = (float)_event.caxis.value / (float)SDL_JOYSTICK_AXIS_MAX;
        if(_event.caxis.axis == 1) _left.y = (float)_event.caxis.value / (float)SDL_JOYSTICK_AXIS_MAX;

        Vec2F _back;
        if(_event.caxis.axis == 2) _back.x = (float)_event.caxis.value / (float)SDL_JOYSTICK_AXIS_MAX;
        if(_event.caxis.axis == 3) _back.y = (float)_event.caxis.value / (float)SDL_JOYSTICK_AXIS_MAX;

        Vec2F _right;
        if(_event.caxis.axis == 4) _right.x = (float)_event.caxis.value / (float)SDL_JOYSTICK_AXIS_MAX;
        if(_event.caxis.axis == 5) _right.y = (float)_event.caxis.value / (float)SDL_JOYSTICK_AXIS_MAX;

        JoystickAxisMovedEvent _e(_left, _right, _back);
        window->consumeEvent(_e);
    }

    void InputSystem::onGamepadsButtonDown(SDL_Event& _event) {
        auto _key = static_cast<GamePadKeys>(_event.cbutton.button);

        JoystickButtonDownEvent _e(_key);
        window->consumeEvent(_e);

        if(pressedGamepadKeys[_key] == 2) return;
        pressedGamepadKeys[_key] = 1;
    }

    void InputSystem::onGamepadsButtonUp(SDL_Event& _event) {
        auto _key = static_cast<GamePadKeys>(_event.cbutton.button);
        pressedGamepadKeys[_key] = 0;

        JoystickButtonUpEvent _e(_key);
        window->consumeEvent(_e);
    }

    InputSystem::~InputSystem() {
        LOG_S("Cleaning up Input System")
//        for(auto* _joystick : joysticks)
//            SDL_JoystickClose(_joystick);
    }




    /// ----------------------- INPUT MANAGER



    InputSystem* InputManager::inputSystem = &InputSystem::get();

    bool InputManager::isKeyJustPressed(KeyCode _keyCode) {
        if(inputSystem->pressedKeyboardKeys[_keyCode] == 1) {
            inputSystem->pressedKeyboardKeys[_keyCode] = 2;
            return true;
        }

        return false;
    }

    bool InputManager::isKeyPressed(KeyCode _key) {
        return inputSystem->pressedKeyboardKeys[_key] == 1;
    }

    bool InputManager::isKeyReleased(KeyCode _key) {
        return inputSystem->pressedKeyboardKeys[_key] == 0;
    }

    bool InputManager::isMouseJustPressed(MouseCode _mouseButton) {
        if(inputSystem->pressedMouseButtons[_mouseButton] == 1) {
            inputSystem->pressedMouseButtons[_mouseButton] = 2;
            return true;
        }

        return false;
    }

    bool InputManager::isMousePressed(MouseCode _button) {
        return inputSystem->pressedMouseButtons[_button] == 1;
    }

    bool InputManager::isMouseReleased(MouseCode _button) {
        return  inputSystem->pressedMouseButtons[_button] == 0;;
    }

    Vec2I InputManager::getMousePosition() {
        return inputSystem->mousePos;
    }

    int InputManager::getMouseX() {
        return getMousePosition().x;
    }

    int InputManager::getMouseY() {
        return getMousePosition().y;
    }

    bool InputManager::isGamepadButtonJustPressed(GamePadKeys _button) {
        if(inputSystem->pressedGamepadKeys[_button] == 1) {
            inputSystem->pressedGamepadKeys[_button] = 2;
            return true;
        }

        return false;
    }

    bool InputManager::isGamepadButtonPressed(GamePadKeys _button) {
        return inputSystem->pressedGamepadKeys[_button] == 1;
    }

    bool InputManager::isGamepadButtonReleased(GamePadKeys _button) {
        return inputSystem->pressedGamepadKeys[_button] == 0;
    }
}