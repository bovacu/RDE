#pragma once

#ifndef ENGINE2_0_INPUT_H
#define ENGINE2_0_INPUT_H

#include "core/platform/PlatformHeaderSDL.h"
#include "engine/include/core/systems/inputSystem/keysAndButtons/KeyboardKeys.h"
#include "engine/include/core/systems/inputSystem/keysAndButtons/MouseKeys.h"
#include "engine/include/core/systems/inputSystem/keysAndButtons/GamePadButtons.h"

#include "core/util/Util.h"
#include "core/render/window/Window.h"

#ifndef SDL_JOYSTICK_DISABLED
#define SDL_JOYSTICK_DISABLED
#endif

namespace GDE {

    enum SystemEventEnum {
        WINDOW_EVENT = SDL_WINDOWEVENT, QUIT_E = SDL_QUIT, WINDOW_EXIT_E = SDL_WINDOWEVENT_LEAVE, WINDOW_RESIZED_E = SDL_WINDOWEVENT_RESIZED,
        WINDOW_FOCUS_E = SDL_WINDOWEVENT_FOCUS_GAINED, WINDOW_LOST_FOCUS_E = SDL_WINDOWEVENT_FOCUS_LOST, WINDOW_AUDIO_DEVICE_CONNECTED_E = SDL_AUDIODEVICEADDED,
        WINDOW_AUDIO_DEVICE_DISCONNECTED_E = SDL_AUDIODEVICEREMOVED,

        APP_ON_DESTROY_E = SDL_APP_TERMINATING, APP_DID_ENTER_BACK_E = SDL_APP_DIDENTERBACKGROUND, APP_DID_ENTER_FOREG_E = SDL_APP_DIDENTERFOREGROUND,
        APP_WILL_ENTER_BACK_E = SDL_APP_WILLENTERBACKGROUND, APP_WILL_ENTER_FOREG = SDL_APP_WILLENTERFOREGROUND,

        KEY_DOWN_E = SDL_KEYDOWN, KEY_UP_E = SDL_KEYUP, KEY_MAP_CHANGED_E = SDL_KEYMAPCHANGED, KEY_TEXT_INPUT_E = SDL_TEXTINPUT,
        KEY_TEXT_EDITING_E = SDL_TEXTEDITING,

        MOUSE_MOVED_E = SDL_MOUSEMOTION, MOUSE_DOWN_E = SDL_MOUSEBUTTONDOWN, MOUSE_UP_E = SDL_MOUSEBUTTONUP,
        MOUSE_SCROLLED_E = SDL_MOUSEWHEEL,

        GAMEPAD_JOYSTICK = SDL_CONTROLLERAXISMOTION, GAMEPAD_BUTTON_DOWN = SDL_CONTROLLERBUTTONDOWN, GAMEPAD_BUTTON_UP = SDL_CONTROLLERBUTTONUP,
        GAMEPAD_CONNECTED_E = SDL_CONTROLLERDEVICEADDED, GAMEPAD_DISCONNECTED_E = SDL_CONTROLLERDEVICEREMOVED,

        JOYSTICK_HAT_MOTION_E = SDL_JOYHATMOTION, JOYSTICK_BALL_MOTION_E = SDL_JOYBALLMOTION, JOYSTICK_BUTTON_DOWN_E = SDL_JOYBUTTONDOWN,
        JOYSTICK_BUTTON_UP_E = SDL_JOYBUTTONUP, JOYSTICK_CONNECTED_E = SDL_JOYDEVICEADDED, JOYSTICK_DISCONNECTED_E = SDL_JOYDEVICEREMOVED,
        JOYSTICK_AXIS_MOTION_E = SDL_JOYAXISMOTION,

        MOBILE_TOUCH_DOWN_E = SDL_FINGERDOWN, MOBILE_TOUCH_UP_E = SDL_FINGERUP, MOBILE_TOUCH_MOVED = SDL_FINGERMOTION,

        UNKNOWN
    };

    enum InputType {
        WINDOW,
        MOUSE,
        KEYBOARD,
        CONTROLLER,
        MOBILE
    };

    class Engine;
    class Manager;

    /// This class is the base for any input specific class that the engine wants to implement.
    class Input {
        friend class InputManager;
        protected:
            Window* window = nullptr;
            std::unordered_map<int, UDelegate<void(SDL_Event&)>> events;
            std::vector<SystemEventEnum> ignoredEvents;
            Engine* engine;

        public:
            bool pollEvent(SDL_Event& _event);
            bool ignoreEvent(const SDL_EventType& _eventType);
    };

    class WindowInput; class KeyboardInput; class MouseInput; class ControllerInput; class MobileInput;
    /// This class allows the user to ask for any user input in a easy and accessible way.
    /// All managers destroy all the resources before closing the application, it is not needed to be done by the end user.
    class InputManager {

        private:
            WindowInput* windowInput;
            KeyboardInput* keyboardInput;
            MouseInput* mouseInput;
            ControllerInput* controllerInput;
            MobileInput* mobileInput;
            Engine* engine;

        public:
            /// This functions should not be called by end users.
            void init(Engine* _engine, Window* _window);

            /// This functions should not be called by end users.
            void destroy();

            /// This functions should not be called by end users.
            void pollEvents();

        public:
            /// Returns true if the key is pressed and won't return true until the key is released and pressed again.
            bool isKeyJustPressed(KeyCode _key);
            /// Returns true if the key is released and won't return true until the key is pressed and released again.
            bool isKeyJustReleased(KeyCode _key);
            /// Returns true while the key is being pressed.
            bool isKeyPressed(KeyCode _key);
            /// Returns true while the key is not being pressed.
            bool isKeyReleased(KeyCode _key);


            /// Returns true if the button is pressed and won't return true until the button is released and pressed again.
            bool isMouseJustPressed(MouseCode _button);
            /// Returns true if the button is released and won't return true until the button is pressed and released again.
            bool isMouseJustReleased(MouseCode _button);
            /// Returns true while the button is being pressed.
            bool isMousePressed(MouseCode _button);
            /// Returns true while the button is not being pressed.
            bool isMouseReleased(MouseCode _button);
            /// Returns the mouse position in screen coordinates, so x: [0, WindowWidth], y: [0, WindowHeight].
            Vec2F getMousePosScreenCoords(bool _centeredMiddleScreen = true);
            /// Returns the mouse position inside the world coordinates system.
            Vec2F getMousePosWorldPos();


            /// This function changes the order of the controllers (use to set who is player1, for example).
            bool reassignController(int _controllerID, int _as);
            /// Returns true if the button is pressed and won't return true until the button is released and pressed again.
            bool isGamepadButtonJustPressed(GamePadButtons _button, int _controllerID = 0);
            /// Returns true if the button is released and won't return true until the button is pressed and released again.
            bool isGamepadButtonJustReleased(GamePadButtons _button, int _controllerID = 0);
            /// Returns true while the button is being pressed.
            bool isGamepadButtonPressed(GamePadButtons _button, int _controllerID = 0);
            /// Returns true while the button is not being pressed.
            bool isGamepadButtonReleased(GamePadButtons _button, int _controllerID = 0);
            /// Makes the controller vibrate at a specific vibration effect.
            bool gamepadVibrate(int _controllerID = 0, const std::string& _vibrationEffectName = "default");


            /// Returns true if the axis button is pressed and won't return true until the axis button is released and pressed again.
            bool isGamepadAxisJustPressed(GamePadAxis _axis, int _controllerID = 0);
            /// Returns true while the axis button is being pressed.
            bool isGamepadAxisPressed(GamePadAxis _axis, int _controllerID = 0);
            /// Returns true while the axis button is not being pressed.
            bool isGamepadAxisReleased(GamePadAxis _axis, int _controllerID = 0);


            /// Returns true if the screen is pressed and won't return true until the screen is released and pressed again.
            bool isMobileScreenJustPressed(int _fingerID);
            /// Returns true if the screen is released and won't return true until the button is screen and released again.
            bool isMobileScreenJustReleased(int _fingerID);
            /// Returns true while the screen is being pressed.
            bool isMobileScreenPressed(int _fingerID);
            /// Returns true while the screen is not being pressed.
            bool isMobileScreenRelease(int _fingerID);

            std::vector<SystemEventEnum> getEventsIgnored(const InputType& _inputType);
            void addEventToIgnore(const InputType& _inputType, const SystemEventEnum& _event);
            void removeEventToIgnore(const InputType& _inputType, const SystemEventEnum& _event);
    };
}

#endif //ENGINE2_0_INPUT_H
