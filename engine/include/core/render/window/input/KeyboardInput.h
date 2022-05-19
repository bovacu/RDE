// Created by borja on 6/2/22.


#ifndef ENGINE2_0_KEYBOARD_INPUT_H
#define ENGINE2_0_KEYBOARD_INPUT_H


#include "core/util/Util.h"
#include "core/render/window/keysAndButtons/KeyboardKeys.h"
#include "core/render/window/input/Input.h"

namespace GDE {

    class KeyboardInput : public Input {
        private:
            std::unordered_map<KeyCode, int>  pressedKeyboardKeys;

        public:
            void init(Engine* _engine, Window* _window);
            int getState(int _keyOrButton);
            void setState(int _keyOrButton, int _state);

        private:
            void onKeyDown(SDL_Event& _event);
            void onKeyUp(SDL_Event& _event);
    };

}


#endif //ENGINE2_0_KEYBOARD_INPUT_H
