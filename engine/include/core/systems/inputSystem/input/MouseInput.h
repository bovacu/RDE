// Created by borja on 6/2/22.


#ifndef RDE_MOUSE_INPUT_H
#define RDE_MOUSE_INPUT_H


#include "core/util/Util.h"
#include "core/systems/inputSystem/keysAndButtons/MouseKeys.h"
#include "core/systems/inputSystem/input/Input.h"

namespace RDE {

    class MouseInput : public Input {
        private:
            /**
             * @brief Map MouseCode -> State.
             */
			std::unordered_map<RDE_MOUSE_BUTTON_, RDE_INPUT_STATUS_>  pressedMouseButtons;

            /**
             * @brief Mouse position in screen.
             */
            Vec2I mousePos;

        public:
            /**
             * @brief Initiates the system and sets the initial states of the buttons.
             * @param _engine Engine
             * @param _window Window
             */
			RDE_FUNC void init(Engine* _engine, Window* _window);

            /**
             * @brief Gets the state of a button.
			 * @param _keyOrButton RDE_MOUSE_BUTTON_
			 * @return RDE_MOUSE_BUTTON_STATUS_
             */
			RDE_FUNC_ND RDE_INPUT_STATUS_ getState(RDE_MOUSE_BUTTON_ _mouseButton);

            /**
             * @brief Sets the state of a button.
			 * @param _keyOrButton RDE_MOUSE_BUTTON_
			 * @param _state RDE_MOUSE_BUTTON_STATUS_
             */
			RDE_FUNC void setState(RDE_MOUSE_BUTTON_ _mouseButton, RDE_INPUT_STATUS_ _state);

            /**
             * @brief Gets the position of the mouse in World Coordinates.
             * @return Vec2F
             */
			RDE_FUNC_ND Vec2F getMousePosition() const;

        private:
            /**
             * @brief Handles the mouse moved event.
             * @param _event Event
             */
            void onMouseMoved(SDL_Event& _event);

            /**
             * @brief Handles the mouse down event.
             * @param _event Event
             */
            void onMouseDown(SDL_Event& _event);

            /**
             * @brief Handles the mouse up event.
             * @param _event Event
             */
            void onMouseUp(SDL_Event& _event);

            /**
             * @brief Handles the mouse scrolled event.
             * @param _event Event
             */
            void onMouseScroll(SDL_Event& _event);
    };

}


#endif //RDE_MOUSE_INPUT_H
