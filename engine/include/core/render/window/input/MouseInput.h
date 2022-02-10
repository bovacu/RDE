// Created by borja on 6/2/22.


#ifndef ENGINE2_0_MOUSE_INPUT_H
#define ENGINE2_0_MOUSE_INPUT_H


#include "core/util/Util.h"
#include "core/render/window/Window.h"
#include "core/render/window/keysAndButtons/MouseKeys.h"
#include "core/render/window/input/Input.h"

namespace engine {

    class MouseInput : public Input {
        private:
            std::unordered_map<MouseCode, int>  pressedMouseButtons;
            Vec2I mousePos;

        public:
            void init(Window* _window);
            int getState(int _keyOrButton);
            void setState(int _keyOrButton, int _state);
            Vec2F getMousePosition() const;

        private:
            void onMouseMoved(SDL_Event& _event);
            void onMouseDown(SDL_Event& _event);
            void onMouseUp(SDL_Event& _event);
            void onMouseScroll(SDL_Event& _event);
    };

}


#endif //ENGINE2_0_MOUSE_INPUT_H
