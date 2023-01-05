// Created by borja on 6/2/22.


#ifndef RDE_KEYBOARD_INPUT_H
#define RDE_KEYBOARD_INPUT_H


#include "core/util/Util.h"
#include "core/systems/inputSystem/keysAndButtons/KeyboardKeys.h"
#include "Input.h"
#include <SDL_events.h>

namespace RDE {

    /**
     * @brief This class handles all of the keyboard inputs.
     */
    class KeyboardInput : public Input {
        private:
            /**
             * @brief Map KeyCode -> State.
             */
            std::unordered_map<KeyCode, int>  pressedKeyboardKeys;

        public:
            /**
             * @brief Initiates the system and sets initial keys states.
             * @param _engine Engine
             * @param _window Window
             */
            void init(Engine* _engine, Window* _window);

            /**
             * @brief Returns the state of a key.
             * @param _keyOrButton Key
             * @return int
             */
            int getState(int _keyOrButton);

            /**
             * @brief Sets the state of a key.
             * @param _keyOrButton Key
             * @param _state State
             */
            void setState(int _keyOrButton, int _state);

        private:
            /**
             * @brief Handles when a key is down.
             * @param _event Event
             */
            void onKeyDown(SDL_Event& _event);

            /**
             * @brief Handles when a key is up.
             * @param _event Event
             */
            void onKeyUp(SDL_Event& _event);

            void onTextTyped(SDL_Event& _event);
    };

}


#endif //RDE_KEYBOARD_INPUT_H
