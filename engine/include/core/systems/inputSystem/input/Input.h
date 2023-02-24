#pragma once

#ifndef RDE_INPUT_H
#define RDE_INPUT_H

#include "core/platform/PlatformHeaderSDL.h"
#include "core/systems/inputSystem/keysAndButtons/KeyboardKeys.h"
#include "core/systems/inputSystem/keysAndButtons/MouseKeys.h"
#include "core/systems/inputSystem/keysAndButtons/ControllerButtons.h"
#include "core/util/Vec.h"
#include "core/render/window/Window.h"
#include "core/Enums.h"
#include <vector>
#include <unordered_map>

#ifndef SDL_JOYSTICK_DISABLED
#define SDL_JOYSTICK_DISABLED
#endif

namespace RDE {

    class Engine; 
    class Manager;
    class WindowInput;
    class KeyboardInput;
    class MouseInput;
    class ControllerInput;
    class MobileInput;

	/**
	 * @brief All of the events that the input system handles.
	*/
	enum RDE_SYSTEM_EVENT_ {
		RDE_SYSTEM_EVENT_WINDOW_EVENT = SDL_WINDOWEVENT, RDE_SYSTEM_EVENT_QUIT_E = SDL_QUIT, RDE_SYSTEM_EVENT_WINDOW_EXIT_E = SDL_WINDOWEVENT_LEAVE, RDE_SYSTEM_EVENT_WINDOW_RESIZED_E = SDL_WINDOWEVENT_RESIZED,
		RDE_SYSTEM_EVENT_WINDOW_FOCUS_E = SDL_WINDOWEVENT_FOCUS_GAINED, RDE_SYSTEM_EVENT_WINDOW_LOST_FOCUS_E = SDL_WINDOWEVENT_FOCUS_LOST, RDE_SYSTEM_EVENT_WINDOW_AUDIO_DEVICE_CONNECTED_E = SDL_AUDIODEVICEADDED,
		RDE_SYSTEM_EVENT_WINDOW_AUDIO_DEVICE_DISCONNECTED_E = SDL_AUDIODEVICEREMOVED,

		RDE_SYSTEM_EVENT_APP_ON_DESTROY_E = SDL_APP_TERMINATING, RDE_SYSTEM_EVENT_APP_DID_ENTER_BACK_E = SDL_APP_DIDENTERBACKGROUND, RDE_SYSTEM_EVENT_APP_DID_ENTER_FOREG_E = SDL_APP_DIDENTERFOREGROUND,
		RDE_SYSTEM_EVENT_APP_WILL_ENTER_BACK_E = SDL_APP_WILLENTERBACKGROUND, RDE_SYSTEM_EVENT_APP_WILL_ENTER_FOREG = SDL_APP_WILLENTERFOREGROUND,

		RDE_SYSTEM_EVENT_KEY_DOWN_E = SDL_KEYDOWN, RDE_SYSTEM_EVENT_KEY_UP_E = SDL_KEYUP, RDE_SYSTEM_EVENT_KEY_MAP_CHANGED_E = SDL_KEYMAPCHANGED, RDE_SYSTEM_EVENT_KEY_TEXT_INPUT_E = SDL_TEXTINPUT,
		RDE_SYSTEM_EVENT_KEY_TEXT_EDITING_E = SDL_TEXTEDITING,

		RDE_SYSTEM_EVENT_MOUSE_MOVED_E = SDL_MOUSEMOTION, RDE_SYSTEM_EVENT_MOUSE_DOWN_E = SDL_MOUSEBUTTONDOWN, RDE_SYSTEM_EVENT_MOUSE_UP_E = SDL_MOUSEBUTTONUP,
		RDE_SYSTEM_EVENT_MOUSE_SCROLLED_E = SDL_MOUSEWHEEL,

		RDE_SYSTEM_EVENT_GAMEPAD_JOYSTICK = SDL_CONTROLLERAXISMOTION, RDE_SYSTEM_EVENT_GAMEPAD_BUTTON_DOWN = SDL_CONTROLLERBUTTONDOWN, RDE_SYSTEM_EVENT_GAMEPAD_BUTTON_UP = SDL_CONTROLLERBUTTONUP,
		RDE_SYSTEM_EVENT_GAMEPAD_CONNECTED_E = SDL_CONTROLLERDEVICEADDED, RDE_SYSTEM_EVENT_GAMEPAD_DISCONNECTED_E = SDL_CONTROLLERDEVICEREMOVED,

		RDE_SYSTEM_EVENT_JOYSTICK_HAT_MOTION_E = SDL_JOYHATMOTION, RDE_SYSTEM_EVENT_JOYSTICK_BALL_MOTION_E = SDL_JOYBALLMOTION, RDE_SYSTEM_EVENT_JOYSTICK_BUTTON_DOWN_E = SDL_JOYBUTTONDOWN,
		RDE_SYSTEM_EVENT_JOYSTICK_BUTTON_UP_E = SDL_JOYBUTTONUP, RDE_SYSTEM_EVENT_JOYSTICK_CONNECTED_E = SDL_JOYDEVICEADDED, RDE_SYSTEM_EVENT_JOYSTICK_DISCONNECTED_E = SDL_JOYDEVICEREMOVED,
		RDE_SYSTEM_EVENT_JOYSTICK_AXIS_MOTION_E = SDL_JOYAXISMOTION,

		RDE_SYSTEM_EVENT_MOBILE_TOUCH_DOWN_E = SDL_FINGERDOWN, RDE_SYSTEM_EVENT_MOBILE_TOUCH_UP_E = SDL_FINGERUP, RDE_SYSTEM_EVENT_MOBILE_TOUCH_MOVED = SDL_FINGERMOTION,

		RDE_SYSTEM_EVENT_UNKNOWN
	};

    /**
     * @brief This class is the base for any input specific class that the engine wants to implement.
     */
    class Input {
        friend class InputManager;
        protected:
            /**
             * @see Window
             */
            Window* window = nullptr;

            /**
             * @brief Map EventID -> Callback for event.
             */
            std::unordered_map<int, UniqueDelegate<void(SDL_Event&)>> events;

            /**
             * @brief Events that the system doesn't handle on purpose.
             */
			std::vector<RDE_SYSTEM_EVENT_> ignoredEvents;

            /**
             * @see Engine
             */
            Engine* engine;

        public:
            /**
             * @brief This method takes a raw SDL event and transforms it to a Engine event and starts the handling process.
             * @param _event SDL event
             * @return bool
             */
            bool pollEvent(SDL_Event& _event);

            /**
             * @brief Checks if an specific event must or not be ignored.
             * @param _eventType Event type
             * @return bool
             */
            bool ignoreEvent(const SDL_EventType& _eventType);
    };

    /**
     * @attention All managers destroy all the resources before closing the application, it is not needed to be done by the end user.
     * @brief This class allows the user to ask for any user input in a easy and accessible way.
     */
    class InputManager {

        private:
            /**
             * @see WindowInput
             */
            WindowInput* windowInput;

            /**
             * @see KeyboardInput
             */
            KeyboardInput* keyboardInput;

            /**
             * @see MouseInput
             */
            MouseInput* mouseInput;

            /**
             * @see ControllerInput
             */
            ControllerInput* controllerInput;

            /**
             * @see MobileInput
             */
            MobileInput* mobileInput;

            /**
             * @see Engine
             */
            Engine* engine;

        public:
            /**
             * @attention This is not meant to be called by end-users.
             * @brief This initiates all of the crucial elements of the input system.
             * @param _engine Engine
             * @param _window Window
             */
            void init(Engine* _engine, Window* _window);

            /**
             * @attention This is not meant to be called by end-users.
             * @brief Destroys all input system elements.
             */
            void destroy();

            /**
             * @attention This is not meant to be called by end-users.
             * @brief Loops over all the captured events by SDL on a frame and handles them.
             */
            void pollEvents();

        public:
            /**
             * @brief Returns true if the key is pressed and won't return true until the key is released and pressed again.
             * @param _key Key pressed
             * @return bool
             */
            bool isKeyJustPressed(RDE_KEYBOARD_KEY_ _key);

            /**
             * @brief Returns true if the key is released and won't return true until the key is pressed and released again.
             * @param _key Key released
             * @return bool
             */
			bool isKeyJustReleased(RDE_KEYBOARD_KEY_ _key);

            /**
             * @brief Returns true while the key is being pressed.
             * @param _key Key pressed
             * @return bool
             */
			bool isKeyPressed(RDE_KEYBOARD_KEY_ _key);

            /**
             * @brief Returns true while the key is not being pressed.
             * @param _key Key released
             * @return bool
             */
			bool isKeyReleased(RDE_KEYBOARD_KEY_ _key);


            /**
             * @brief Returns true if the button is pressed and won't return true until the button is released and pressed again.
             * @param _button Mouse button pressed
             * @return bool
             */
            bool isMouseJustPressed(RDE_MOUSE_BUTTON_ _button);

            /**
             * @brief Returns true if the button is released and won't return true until the button is pressed and released again.
             * @param _button Mouse button released
             * @return bool
             */
			bool isMouseJustReleased(RDE_MOUSE_BUTTON_ _button);

            /**
             * @brief Returns true while the button is being pressed.
             * @param _button Mouse button pressed
             * @return bool
             */
			bool isMousePressed(RDE_MOUSE_BUTTON_ _button);

            /**
             * @brief Returns true while the button is not being pressed.
             * @param _button Mouse button released
             * @return bool
             */
			bool isMouseReleased(RDE_MOUSE_BUTTON_ _button);

            /**
             * @brief Returns the mouse position in screen coordinates, so x: [0, WindowWidth], y: [0, WindowHeight].
             * @param _centeredMiddleScreen Centered on screen center or left-bottom corner
             * @return Vec2F
             */
            Vec2F getMousePosScreenCoords(bool _centeredMiddleScreen = true);

            /**
             * @brief Returns the mouse position inside the world coordinates system.
             * @return Vec2F
             */
            Vec2F getMousePosWorldPos();

            /**
             * @brief Returns mouse position with Canvas camera zoom.
             * @return Vec2f
             */
            Vec2F getMousePosCanvas();

            /**
             * @brief This function changes the order of the controllers (use to set who is player1, for example).
             * @param _controllerID Current ID
             * @param _as New ID
             * @return bool
             */
            bool reassignController(int _controllerID, int _as);

            /**
             * @brief Returns true if the button is pressed and won't return true until the button is released and pressed again.
             * @param _button Button pressed
             * @param _controllerID Controller ID
             * @return bool
             */
            bool isGamepadButtonJustPressed(RDE_CONTROLLER_BUTTON_ _button, int _controllerID = 0);

            /**
             * @brief Returns true if the button is released and won't return true until the button is pressed and released again.
             * @param _button Button released
             * @param _controllerID Controller ID
             * @return bool
             */
			bool isGamepadButtonJustReleased(RDE_CONTROLLER_BUTTON_ _button, int _controllerID = 0);

            /**
             * @brief Returns true while the button is being pressed.
             * @param _button Button pressed
             * @param _controllerID Controller ID
             * @return bool
             */
			bool isGamepadButtonPressed(RDE_CONTROLLER_BUTTON_ _button, int _controllerID = 0);

            /**
             * @brief Returns true while the button is not being pressed.
             * @param _button Button released
             * @param _controllerID Controller ID
             * @return bool
             */
			bool isGamepadButtonReleased(RDE_CONTROLLER_BUTTON_ _button, int _controllerID = 0);

            /**
             * @brief Makes the controller vibrate at a specific vibration effect.
             * @param _controllerID Controller ID
             * @param _vibrationEffectName Vibration effect
             * @return bool
             */
            bool gamepadVibrate(int _controllerID = 0, const std::string& _vibrationEffectName = "default");


            /**
             * @brief Returns true if the axis button is pressed and won't return true until the axis button is released and pressed again.
             * @param _axis Controller Axis
             * @param _controllerID Controller ID
             * @return bool
             */
            bool isGamepadAxisJustPressed(RDE_CONTROLLER_AXIS_ _axis, int _controllerID = 0);

            /**
             * @brief Returns true while the axis button is being pressed.
             * @param _axis Controller Axis
             * @param _controllerID Controller ID
             * @return bool
             */
			bool isGamepadAxisPressed(RDE_CONTROLLER_AXIS_ _axis, int _controllerID = 0);

            /**
             * @brief Returns true while the axis button is not being pressed.
             * @param _axis Controller axis
             * @param _controllerID Controller ID
             * @return bool
             */
			bool isGamepadAxisReleased(RDE_CONTROLLER_AXIS_ _axis, int _controllerID = 0);


            /**
             * @brief Returns true if the screen is pressed and won't return true until the screen is released and pressed again.
             * @param _fingerID Finger ID
             * @return bool
             */
            bool isMobileScreenJustPressed(int _fingerID);

            /**
             * @brief Returns true if the screen is released and won't return true until the button is screen and released again.
             * @param _fingerID Finger ID
             * @return bool
             */
            bool isMobileScreenJustReleased(int _fingerID);

            /**
             * @brief Returns true while the screen is being pressed.
             * @param _fingerID Finger ID
             * @returnbool
             */
            bool isMobileScreenPressed(int _fingerID);

            /**
             * @brief Returns true while the screen is not being pressed.
             * @param _fingerID Finger ID
             * @return bool
             */
            bool isMobileScreenRelease(int _fingerID);

            /**
             * @brief Returns the list of ignored events for a platform.
             * @param _inputType Type of input.
             * @return std::vector<SystemEventEnum>
             */
			std::vector<RDE_SYSTEM_EVENT_> getEventsIgnored(const RDE_INPUT_TYPE_& _inputType);

            /**
             * @brief Adds an event as ignored.
             * @param _inputType Input type
             * @param _event Event type
             */
			void addEventToIgnore(const RDE_INPUT_TYPE_& _inputType, const RDE_SYSTEM_EVENT_& _event);

            /**
             * @brief Removes and event as ignored.
             * @param _inputType Input type
             * @param _event Event type
             */
			void removeEventToIgnore(const RDE_INPUT_TYPE_& _inputType, const RDE_SYSTEM_EVENT_& _event);
    };
}

#endif //RDE_INPUT_H
