// Created by borja on 6/2/22.


#ifndef ENGINE2_0_MOUSE_INPUT_H
#define ENGINE2_0_MOUSE_INPUT_H


#include "core/util/Util.h"
#include "core/systems/inputSystem/keysAndButtons/MouseKeys.h"
#include "Input.h"

namespace GDE {

    class MouseInput : public Input {
        private:
            /**
             * @brief Map MouseCode -> State.
             */
            std::unordered_map<MouseCode, int>  pressedMouseButtons;

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
            void init(Engine* _engine, Window* _window);

            /**
             * @brief Gets the state of a button.
             * @param _keyOrButton Button
             * @return int
             */
            int getState(int _keyOrButton);

            /**
             * @brief Sets the state of a button.
             * @param _keyOrButton Button
             * @param _state State
             */
            void setState(int _keyOrButton, int _state);

            /**
             * @brief Gets the position of the mouse in World Coordinates.
             * @return Vec2F
             */
            Vec2F getMousePosition() const;

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


#endif //ENGINE2_0_MOUSE_INPUT_H
