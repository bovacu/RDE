// Created by borja on 6/2/22.


#ifndef ENGINE2_0_WINDOW_INPUT_H
#define ENGINE2_0_WINDOW_INPUT_H


#include "engine/include/core/util/Util.h"
#include "engine/include/core/render/window/Window.h"

namespace engine {

    class WindowInput {
        private:
            Window* window = nullptr;
            std::unordered_map<int, std::function<void(SDL_Event&)>> events;

        public:
            void init(Window* _window);
            bool pollEvent(SDL_Event& _event);

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
