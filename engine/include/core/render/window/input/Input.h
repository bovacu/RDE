#pragma once

#ifndef ENGINE2_0_INPUT_H
#define ENGINE2_0_INPUT_H

#include "core/platform/PlatformHeaderSDL.h"
#include "core/render/window/keysAndButtons/KeyboardKeys.h"
#include "core/render/window/keysAndButtons/MouseKeys.h"
#include "core/render/window/keysAndButtons/GamePadButtons.h"

#include "core/util/Util.h"
#include "core/render/window/Window.h"

#ifndef SDL_JOYSTICK_DISABLED
#define SDL_JOYSTICK_DISABLED
#endif

namespace engine {

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

    class Input {
        friend class InputManager;
        protected:
            Window* window = nullptr;
            std::unordered_map<int, UDelegate<void(SDL_Event&)>> events;
            std::vector<SystemEventEnum> ignoredEvents;

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

        public:
            static InputManager& get();
            void init(Window* _window);
            void destroy();
            void pollEvents();

        public:
            static bool isKeyJustPressed(KeyCode _key);
            static bool isKeyJustReleased(KeyCode _key);
            static bool isKeyPressed(KeyCode _key);
            static bool isKeyReleased(KeyCode _key);


            static bool isMouseJustPressed(MouseCode _button);
            static bool isMouseJustReleased(MouseCode _button);
            static bool isMousePressed(MouseCode _button);
            static bool isMouseReleased(MouseCode _button);
            static Vec2F getMousePosScreenCoords(bool _centeredMiddleScreen = true);
            static Vec2F getMousePosWorldPos();


            static bool reassignController(int _controllerID, int _as);
            static bool isGamepadButtonJustPressed(GamePadButtons _button, int _controllerID = 0);
            static bool isGamepadButtonJustReleased(GamePadButtons _button, int _controllerID = 0);
            static bool isGamepadButtonPressed(GamePadButtons _button, int _controllerID = 0);
            static bool isGamepadButtonReleased(GamePadButtons _button, int _controllerID = 0);
            static bool gamepadVibrate(int _controllerID = 0, const std::string& _vibrationEffectName = "default");


            static bool isGamepadAxisJustPressed(GamePadAxis _axis, int _controllerID = 0);
            static bool isGamepadAxisPressed(GamePadAxis _axis, int _controllerID = 0);
            static bool isGamepadAxisReleased(GamePadAxis _axis, int _controllerID = 0);

            static bool isMobileScreenJustPressed(int _fingerID);
            static bool isMobileScreenJustReleased(int _fingerID);
            static bool isMobileScreenPressed(int _fingerID);
            static bool isMobileScreenUp(int _fingerID);

            static std::vector<SystemEventEnum> getEventsIgnored(const InputType& _inputType);
            static void addEventToIgnore(const InputType& _inputType, const SystemEventEnum& _event);
            static void removeEventToIgnore(const InputType& _inputType, const SystemEventEnum& _event);
    };
}

#endif //ENGINE2_0_INPUT_H
