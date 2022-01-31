#pragma once

#ifndef ENGINE2_0_INPUT_H
#define ENGINE2_0_INPUT_H

#include <SDL2/SDL_events.h>
#include "KeyboardKeys.h"
#include "MouseKeys.h"
#include "GamePadKeys.h"
#include <functional>

#include "core/util/Util.h"
#include "core/render/window/Window.h"

namespace engine {

    enum SystemEventEnum {
        WINDOW_EVENT = SDL_WINDOWEVENT, KEY_DOWN_E = SDL_KEYDOWN, KEY_UP_E = SDL_KEYUP, QUIT_E = SDL_QUIT,
        MOUSE_MOVED_E = SDL_MOUSEMOTION, MOUSE_DOWN_E = SDL_MOUSEBUTTONDOWN, MOUSE_UP_E = SDL_MOUSEBUTTONUP,
        MOUSE_SCROLLED_E = SDL_MOUSEWHEEL, WINDOW_EXIT_E = SDL_WINDOWEVENT_LEAVE, WINDOW_RESIZED_E = SDL_WINDOWEVENT_RESIZED,
        WINDOW_FOCUS_E = SDL_WINDOWEVENT_FOCUS_GAINED, WINDOW_LOST_FOCUS_E = SDL_WINDOWEVENT_FOCUS_LOST,
        INPUT_TEXT_E = SDL_TEXTINPUT,
        FOO_1 = SDL_KEYMAPCHANGED, FOO_2 = SDL_JOYAXISMOTION, FOO_3 = SDL_AUDIODEVICEADDED,
        GAMEPAD_JOYSTICK = SDL_CONTROLLERAXISMOTION, GAMEPAD_BUTTON_DOWN = SDL_CONTROLLERBUTTONDOWN, GAMEPAD_BUTTON_UP = SDL_CONTROLLERBUTTONUP
    };

    class InputManager;
    class InputSystem {

        friend class InputManager;

        private:
            std::unordered_map<KeyCode, int>  pressedKeyboardKeys;
            std::unordered_map<MouseCode, int>  pressedMouseButtons;
            std::unordered_map<GamePadKeys, int>  pressedGamepadKeys;
            std::vector<SDL_Joystick*> joysticks;
            std::unordered_map<SystemEventEnum, std::function<void(SDL_Event&)>> events;
            Window* window = nullptr;
            Vec2I mousePos;

            std::function<void(Event&)> eventCallback;

        private:
            InputSystem() = default;
            void onKeyDown(SDL_Event& _event);
            void onKeyUp(SDL_Event& _event);
            void onMouseMoved(SDL_Event& _event);
            void onMouseDown(SDL_Event& _event);
            void onMouseUp(SDL_Event& _event);
            void onMouseScroll(SDL_Event& _event);

            void onWindowEvent(SDL_Event& _event);
            void onWindowEnter(SDL_Event& _event);
            void onWindowExit(SDL_Event& _event);
            void onWindowGainFocus(SDL_Event& _event);
            void onWindowLostFocus(SDL_Event& _event);
            void onWindowResize(SDL_Event& _event);
            void onWindowMoved(SDL_Event& _event);
            void onWindowMinimized(SDL_Event& _event);
            void onWindowMaximized(SDL_Event& _event);
            void onQuit(SDL_Event& _event);

            void initGamepads();
            void onGamepadsMoved(SDL_Event& _event);
            void onGamepadsButtonDown(SDL_Event& _event);
            void onGamepadsButtonUp(SDL_Event& _event);

        public:
            void init(Window* _window);
            void pollEvents();
            void setEventCallback(std::function<void(Event&)> _eventCallback);

            static InputSystem& get();
            ~InputSystem();
    };

    class InputManager {

        private:
            static InputSystem* inputSystem;

        public:
            /// Checks if a key has just been pressed, only the first pressed, if continuous pressing it doesn't detect.
            /// @param _keyCode The code of the key.
            /// @return true if it is just pressed.
            static bool isKeyJustPressed(KeyCode _keyCode);

            /// Checks if a key is being pressed.
            /// @param _keyCode The code of the key.
            /// @return true if it is being pressed.
            static bool isKeyPressed(KeyCode _keyCode);

            /// Checks if a key has been released.
            /// @param _keyCode The code of the key.
            /// @return true if it is been released.
            static bool isKeyReleased(KeyCode _keyCode);

            /// Checks if a mouse button has just been pressed, only the first pressed, if continuous pressing it doesn't detect.
            /// @param _mouseButton The code of the button.
            /// @return true if it is just pressed.
            static bool isMouseJustPressed(MouseCode _mouseButton);

            /// Checks if a mouse button is being pressed.
            /// @param _mouseButton The code of the button.
            /// @return true if it is being pressed.
            static bool isMousePressed(MouseCode _mouseButton);

            /// Checks if a mouse button has been released.
            /// @param _mouseButton The code of the button.
            /// @return true if it is been released.
            static bool isMouseReleased(MouseCode _mouseButton);

            /// Returns the position of the mouse on the screen.
            /// @return Vec2i with [x,y] of the mouse.
            static Vec2I getMousePosition();

            /// Returns getMousePosition().x
            /// @return getMousePosition().x
            static int getMouseX();

            /// Returns getMousePosition().y
            /// @return getMousePosition().y
            static int getMouseY();

            static bool isGamepadButtonJustPressed(GamePadKeys _button);
            static bool isGamepadButtonPressed(GamePadKeys _button);
            static bool isGamepadButtonReleased(GamePadKeys _button);
    };
}

#endif //ENGINE2_0_INPUT_H
