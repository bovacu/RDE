// Created by borja on 6/2/22.


#ifndef ENGINE2_0_WINDOW_INPUT_H
#define ENGINE2_0_WINDOW_INPUT_H


#include "core/util/Util.h"
#include "Input.h"

namespace GDE {

    class WindowInput : public Input {
        public:
            void init(Engine* _engine, Window* _window);

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

            void onDestroyApp(SDL_Event& _event);
            void onDidEnterForegroundApp(SDL_Event& _event);
            void onDidEnterBackground(SDL_Event& _event);
            void onWillEnterForegroundApp(SDL_Event& _event);
            void onWillEnterBackground(SDL_Event& _event);
    };

}


#endif //ENGINE2_0_WINDOW_INPUT_H
