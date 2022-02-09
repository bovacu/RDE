// Created by borja on 6/2/22.


#ifndef ENGINE2_0_KEYBOARD_INPUT_H
#define ENGINE2_0_KEYBOARD_INPUT_H


#include "engine/include/core/util/Util.h"
#include "engine/include/core/render/window/keysAndButtons/KeyboardKeys.h"
#include "engine/include/core/render/window/Window.h"

namespace engine {

    class KeyboardInput {
        private:
            std::unordered_map<KeyCode, int>  pressedKeyboardKeys;
            Window* window = nullptr;
            std::unordered_map<int, std::function<void(SDL_Event&)>> events;

        public:
            void init(Window* _window);
            bool pollEvent(SDL_Event& _event);
            int getState(int _keyOrButton);
            void setState(int _keyOrButton, int _state);

        private:
            void onKeyDown(SDL_Event& _event);
            void onKeyUp(SDL_Event& _event);
    };

}


#endif //ENGINE2_0_KEYBOARD_INPUT_H
