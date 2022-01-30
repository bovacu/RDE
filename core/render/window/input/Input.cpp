#include "Input.h"
#include "core/Engine.h"
#include <GLFW/glfw3.h>

namespace engine {

    InputSystem& InputSystem::get() {
        static InputSystem _input;
        return _input;
    }

    void InputSystem::init(Window* _window) {

        window = _window;

        events[SystemEventEnum::KEY_DOWN_E] = BIND_FUNC_1(InputSystem::onKeyDown);
        events[SystemEventEnum::KEY_UP_E] = BIND_FUNC_1(InputSystem::onKeyUp);
        events[SystemEventEnum::WINDOW_RESIZE_E] = BIND_FUNC_1(InputSystem::onWindowResize);
        events[SystemEventEnum::QUIT_E] = BIND_FUNC_1(InputSystem::onQuit);
        events[SystemEventEnum::MOUSE_MOVED_E] = BIND_FUNC_1(InputSystem::onMouseMoved);
        events[SystemEventEnum::MOUSE_DOWN_E] = BIND_FUNC_1(InputSystem::onMouseDown);
        events[SystemEventEnum::MOUSE_UP_E] = BIND_FUNC_1(InputSystem::onMouseUp);
        events[SystemEventEnum::MOUSE_SCROLLED_E] = BIND_FUNC_1(InputSystem::onMouseScroll);
        events[SystemEventEnum::WINDOW_ENTER_E] = [this](SDL_Event&) { insideWindow = true; };
        events[SystemEventEnum::WINDOW_EXIT_E] = [this](SDL_Event&) { insideWindow = false; };
        events[SystemEventEnum::WINDOW_FOCUS_E] = [this](SDL_Event&) { LOG_I("Gained Focus") };
        events[SystemEventEnum::WINDOW_LOST_FOCUS_E] = [this](SDL_Event&) { LOG_I("Lost Focus") };

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

    void InputSystem::onKeyDown(SDL_Event& _event) {
        auto _key = static_cast<KeyCode>(_event.key.keysym.scancode);
        if(pressedKeyboardKeys[_key] == 2) return;
        pressedKeyboardKeys[_key] = 1;
    }

    void InputSystem::onKeyUp(SDL_Event& _event) {
        auto _key = static_cast<KeyCode>(_event.key.keysym.scancode);
        pressedKeyboardKeys[_key] = 0;
    }

    void InputSystem::onWindowResize(SDL_Event& _event) {
        window->setWindowSize(_event.window.data1, _event.window.data2);
    }

    void InputSystem::onQuit(SDL_Event& _event) {
        Engine::get().setRunning(false);
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

    }


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
}