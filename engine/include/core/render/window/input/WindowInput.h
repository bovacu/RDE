// Created by borja on 6/2/22.


#ifndef ENGINE2_0_WINDOW_INPUT_H
#define ENGINE2_0_WINDOW_INPUT_H


#include "core/util/Util.h"
#include "core/render/window/Window.h"
#include "core/render/window/input/Input.h"

namespace engine {

    class WindowInput : public Input {

        public:
            void init(Window* _window);

        private:
            void onWindowEvent(SDL_Event& _event);
            void onWindowEnter(SDL_Event& _event);
            void onWindowExit(SDL_Event& _event);
            void onWindowGainFocus(SDL_Event& _event);
            void onWindowLostFocus(SDL_Event& _event);
            void onWindowResize(SDL_Event& _event);
            void onWindowMoved(SDL_Event& _event);
            void onWindowMinimized(SDL_Event& _event);
            void onWindowMaximized(SDL_Event& _event);
            void onQuit(SDL_Event& _event);
    };

}


#endif //ENGINE2_0_WINDOW_INPUT_H
