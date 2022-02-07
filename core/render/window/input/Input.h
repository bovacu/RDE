#pragma once

#ifndef ENGINE2_0_INPUT_H
#define ENGINE2_0_INPUT_H

#include <SDL2/SDL_events.h>
#include "core/render/window/keysAndButtons/KeyboardKeys.h"
#include "core/render/window/keysAndButtons/MouseKeys.h"
#include "core/render/window/keysAndButtons/GamePadKeys.h"
#include <functional>

#include "core/util/Util.h"
#include "core/render/window/Window.h"
#include "WindowInput.h"
#include "KeyboardInput.h"
#include "MouseInput.h"
#include "ControllerInput.h"

#define SDL_JOYSTICK_DISABLED

namespace engine {

    enum SystemEventEnum {
        WINDOW_EVENT = SDL_WINDOWEVENT, KEY_DOWN_E = SDL_KEYDOWN, KEY_UP_E = SDL_KEYUP, QUIT_E = SDL_QUIT,
        MOUSE_MOVED_E = SDL_MOUSEMOTION, MOUSE_DOWN_E = SDL_MOUSEBUTTONDOWN, MOUSE_UP_E = SDL_MOUSEBUTTONUP,
        MOUSE_SCROLLED_E = SDL_MOUSEWHEEL, WINDOW_EXIT_E = SDL_WINDOWEVENT_LEAVE, WINDOW_RESIZED_E = SDL_WINDOWEVENT_RESIZED,
        WINDOW_FOCUS_E = SDL_WINDOWEVENT_FOCUS_GAINED, WINDOW_LOST_FOCUS_E = SDL_WINDOWEVENT_FOCUS_LOST,
        INPUT_TEXT_E = SDL_TEXTINPUT,
        FOO_1 = SDL_KEYMAPCHANGED, FOO_2 = SDL_JOYAXISMOTION, FOO_3 = SDL_AUDIODEVICEADDED,
        GAMEPAD_JOYSTICK = SDL_CONTROLLERAXISMOTION, GAMEPAD_BUTTON_DOWN = SDL_CONTROLLERBUTTONDOWN, GAMEPAD_BUTTON_UP = SDL_CONTROLLERBUTTONUP,
        GAMEPAD_CONNECTED_E = SDL_CONTROLLERDEVICEADDED, GAMEPAD_DISCONNECTED_E = SDL_CONTROLLERDEVICEREMOVED
    };

    class InputManager {

        private:
            WindowInput windowInput;
            KeyboardInput keyboardInput;
            MouseInput mouseInput;
            ControllerInput controllerInput;
            std::function<void(Event&)> eventCallback;

        public:
            static InputManager& get();
            void init(Window* _window);
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
    };
}

#endif //ENGINE2_0_INPUT_H
