// Created by borja on 6/2/22.


#ifndef RDE_KEYBOARD_INPUT_H
#define RDE_KEYBOARD_INPUT_H


#include "core/util/Util.h"
#include "core/systems/inputSystem/keysAndButtons/KeyboardKeys.h"
#include "core/systems/inputSystem/input/Input.h"
#include "core/platform/PlatformHeaderSDL.h"

namespace RDE {

    /**
     * @brief This class handles all of the keyboard inputs.
     */
    class KeyboardInput : public Input {
        private:
            /**
             * @brief Map KeyCode -> State.
             */
            std::unordered_map<RDE_KEYBOARD_KEY_, int>  pressedKeyboardKeys;

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

            /**
             * @brief Gets the char that was typed.
             * @param _event Event
            */
            void onTextTyped(SDL_Event& _event);
    };

}


#endif //RDE_KEYBOARD_INPUT_H
