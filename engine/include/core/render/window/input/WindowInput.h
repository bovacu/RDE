// Created by borja on 6/2/22.


#ifndef ENGINE2_0_WINDOW_INPUT_H
#define ENGINE2_0_WINDOW_INPUT_H


#include "core/util/Util.h"
#include "core/render/window/input/Input.h"

namespace GDE {

    class WindowInput : public Input {
        public:
            void init(Window* _window);

        private:
            void onWindowEvent(SDL_Event& _event, RmlData* _rmlData);
            void onWindowEnter(SDL_Event& _event, RmlData* _rmlData);
            void onWindowExit(SDL_Event& _event, RmlData* _rmlData);
            void onWindowGainFocus(SDL_Event& _event, RmlData* _rmlData);
            void onWindowLostFocus(SDL_Event& _event, RmlData* _rmlData);
            void onWindowResize(SDL_Event& _event, RmlData* _rmlData);
            void onWindowMoved(SDL_Event& _event, RmlData* _rmlData);
            void onWindowMinimized(SDL_Event& _event, RmlData* _rmlData);
            void onWindowMaximized(SDL_Event& _event, RmlData* _rmlData);
            void onQuit(SDL_Event& _event, RmlData* _rmlData);

            void onDestroyApp(SDL_Event& _event, RmlData* _rmlData);
            void onDidEnterForegroundApp(SDL_Event& _event, RmlData* _rmlData);
            void onDidEnterBackground(SDL_Event& _event, RmlData* _rmlData);
            void onWillEnterForegroundApp(SDL_Event& _event, RmlData* _rmlData);
            void onWillEnterBackground(SDL_Event& _event, RmlData* _rmlData);
    };

}


#endif //ENGINE2_0_WINDOW_INPUT_H
