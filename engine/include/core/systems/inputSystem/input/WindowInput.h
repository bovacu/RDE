// Created by borja on 6/2/22.


#ifndef RDE_WINDOW_INPUT_H
#define RDE_WINDOW_INPUT_H


#include "core/util/Util.h"
#include "core/systems/inputSystem/input/Input.h"

namespace RDE {

    class WindowInput : public Input {
        public:
            /**
             * @brief Initiates the system and the initial states.
             * @param _engine Engine
             * @param _window Window
             */
            void init(Engine* _engine, Window* _window);

        private:
            /**
             * @brief Is the Hub for all the sb-events.
             * @param _event SDL_event captured by SDL.
             */
            void onWindowEvent(SDL_Event& _event);

            /**
             * @brief Handles when the mouse enters the window.
             * @param _event SDL_event captured by SDL.
             */
            void onWindowEnter(SDL_Event& _event);

            /**
             * @brief Handles when the mouse leaves the window.
             * @param _event SDL_event captured by SDL.
             */
            void onWindowExit(SDL_Event& _event);

            /**
             * @brief Handles when the window gains focus.
             * @param _event SDL_event captured by SDL.
             */
            void onWindowGainFocus(SDL_Event& _event);

            /**
            * @brief Handles when the window looses focus.
            * @param _event SDL_event captured by SDL.
            */
            void onWindowLostFocus(SDL_Event& _event);

            /**
            * @brief Handles when the window is resized.
            * @param _event SDL_event captured by SDL.
            */
            void onWindowResize(SDL_Event& _event);

            /**
            * @brief Handles when the window is moved.
            * @param _event SDL_event captured by SDL.
            */
            void onWindowMoved(SDL_Event& _event);

            /**
            * @brief Handles when the window is changed of display. SDL does not provide a direct event for this, so we could call this
            * 'an artifical' even, this is why the parameter doesn't match the others.
            * @param _event int
            */
            void onWindowDisplayChanged(int _newWindowDisplay);

            /**
            * @brief Handles when the window is minimized.
            * @param _event SDL_event captured by SDL.
            */
            void onWindowMinimized(SDL_Event& _event);

            /**
            * @brief Handles when the window is maximized.
            * @param _event SDL_event captured by SDL.
            */
            void onWindowMaximized(SDL_Event& _event);

            /**
            * @brief Handles when the window is closed.
            * @param _event SDL_event captured by SDL.
            */
            void onQuit(SDL_Event& _event);

            /**
             * @brief Handles Mobile application quit.
             * @param _event SDL_event captured by SDL.
             */
            void onDestroyApp(SDL_Event& _event);

            /**
             * @brief Handles Mobile did enter foreground.
             * @param _event SDL_event captured by SDL.
             */
            void onDidEnterForegroundApp(SDL_Event& _event);

            /**
             * @brief Handles Mobile did enter background.
             * @param _event SDL_event captured by SDL.
             */
            void onDidEnterBackground(SDL_Event& _event);

            /**
             * @brief Handles Mobile will enter foreground.
             * @param _event SDL_event captured by SDL.
             */
            void onWillEnterForegroundApp(SDL_Event& _event);

            /**
             * @brief Handles Mobile will enter background.
             * @param _event SDL_event captured by SDL.
             */
            void onWillEnterBackground(SDL_Event& _event);
    };

}


#endif //RDE_WINDOW_INPUT_H
