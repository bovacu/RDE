#pragma once

#ifndef ENGINE2_0_INPUT_H
#define ENGINE2_0_INPUT_H

#include "core/platform/PlatformHeaderSDL.h"
#include "core/render/window/keysAndButtons/KeyboardKeys.h"
#include "core/render/window/keysAndButtons/MouseKeys.h"
#include "core/render/window/keysAndButtons/GamePadKeys.h"
#include <functional>

#include "core/util/Util.h"
#include "core/render/window/Window.h"

#define SDL_JOYSTICK_DISABLED

namespace engine {

    enum SystemEventEnum {
        WINDOW_EVENT = SDL_WINDOWEVENT, KEY_DOWN_E = SDL_KEYDOWN, KEY_UP_E = SDL_KEYUP, QUIT_E = SDL_QUIT,
        MOUSE_MOVED_E = SDL_MOUSEMOTION, MOUSE_DOWN_E = SDL_MOUSEBUTTONDOWN, MOUSE_UP_E = SDL_MOUSEBUTTONUP,
        MOUSE_SCROLLED_E = SDL_MOUSEWHEEL, WINDOW_EXIT_E = SDL_WINDOWEVENT_LEAVE, WINDOW_RESIZED_E = SDL_WINDOWEVENT_RESIZED,
        WINDOW_FOCUS_E = SDL_WINDOWEVENT_FOCUS_GAINED, WINDOW_LOST_FOCUS_E = SDL_WINDOWEVENT_FOCUS_LOST,
        INPUT_TEXT_E = SDL_TEXTINPUT,

        GAMEPAD_JOYSTICK = SDL_CONTROLLERAXISMOTION, GAMEPAD_BUTTON_DOWN = SDL_CONTROLLERBUTTONDOWN, GAMEPAD_BUTTON_UP = SDL_CONTROLLERBUTTONUP,
        GAMEPAD_CONNECTED_E = SDL_CONTROLLERDEVICEADDED, GAMEPAD_DISCONNECTED_E = SDL_CONTROLLERDEVICEREMOVED,

        MOBILE_TOUCH_DOWN_E = SDL_FINGERDOWN, MOBILE_TOUCH_UP_E = SDL_FINGERUP, MOBILE_TOUCH_MOVED = SDL_FINGERMOTION
    };

    class Input {
        protected:
            Window* window = nullptr;
            std::unordered_map<int, std::function<void(SDL_Event&)>> events;
            std::vector<SDL_EventType> ignoredEvents;

        public:
            bool pollEvent(SDL_Event& _event);
            bool ignoreEvent(const SDL_EventType& _eventType);
    };

    class WindowInput; class KeyboardInput; class MouseInput; class ControllerInput; class MobileInput;
    class InputManager {

        private:
            WindowInput* windowInput;
            KeyboardInput* keyboardInput;
            MouseInput* mouseInput;
            ControllerInput* controllerInput;
            MobileInput* mobileInput;
            std::function<void(Event&)> eventCallback;

        public:
            static InputManager& get();
            void init(Window* _window);
            void destroy();
            void pollEvents();
            void setEventCallback(std::function<void(Event&)> _eventCallback);

        public:
            static bool isKeyJustPressed(KeyCode _key);
            static bool isKeyPressed(KeyCode _key);
            static bool isKeyReleased(KeyCode _key);


            static bool isMouseJustPressed(MouseCode _button);
            static bool isMousePressed(MouseCode _button);
            static bool isMouseReleased(MouseCode _button);
            static Vec2F getMousePosition();
            static int getMouseX();
            static int getMouseY();


            static bool isGamepadButtonJustPressed(GamePadKeys _button);
            static bool isGamepadButtonPressed(GamePadKeys _button);
            static bool isGamepadButtonReleased(GamePadKeys _button);


            static bool isGamepadAxisJustPressed(GamePadAxis _axis);
            static bool isGamepadAxisPressed(GamePadAxis _axis);
            static bool isGamepadAxisReleased(GamePadAxis _axis);

            static bool isMobileScreenJustPressed(int _fingerID);
            static bool isMobileScreenPressed(int _fingerID);
            static bool isMobileScreenUp(int _fingerID);
    };
}

#endif //ENGINE2_0_INPUT_H
