// Created by borja on 6/2/22.


#ifndef ENGINE2_0_WINDOW_INPUT_H
#define ENGINE2_0_WINDOW_INPUT_H


#include "core/util/Util.h"
#include "Input.h"

namespace GDE {

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
             * @param _event
             */
            void onWindowEvent(SDL_Event& _event);

            /**
             * @brief Handles when the mouse enters the window.
             * @param _event Event
             */
            void onWindowEnter(SDL_Event& _event);

            /**
             * @brief Handles when the mouse leaves the window.
             * @param _event Event
             */
            void onWindowExit(SDL_Event& _event);

            /**
             * @brief Handles when the window gains focus.
             * @param _event Event
             */
            void onWindowGainFocus(SDL_Event& _event);

            /**
            * @brief Handles when the window looses focus.
            * @param _event Event
            */
            void onWindowLostFocus(SDL_Event& _event);

            /**
            * @brief Handles when the window is resized.
            * @param _event Event
            */
            void onWindowResize(SDL_Event& _event);

            /**
            * @brief Handles when the window is moved.
            * @param _event Event
            */
            void onWindowMoved(SDL_Event& _event);

            /**
            * @brief Handles when the window is minimized.
            * @param _event Event
            */
            void onWindowMinimized(SDL_Event& _event);

            /**
            * @brief Handles when the window is maximized.
            * @param _event Event
            */
            void onWindowMaximized(SDL_Event& _event);

            /**
            * @brief Handles when the window is closed.
            * @param _event Event
            */
            void onQuit(SDL_Event& _event);

            /**
             * @brief Handles Mobile application quit.
             * @param _event Event
             */
            void onDestroyApp(SDL_Event& _event);

            /**
             * @brief Handles Mobile did enter foreground.
             * @param _event Event
             */
            void onDidEnterForegroundApp(SDL_Event& _event);

            /**
             * @brief Handles Mobile did enter background.
             * @param _event Event
             */
            void onDidEnterBackground(SDL_Event& _event);

            /**
             * @brief Handles Mobile will enter foreground.
             * @param _event Event
             */
            void onWillEnterForegroundApp(SDL_Event& _event);

            /**
             * @brief Handles Mobile will enter background.
             * @param _event Event
             */
            void onWillEnterBackground(SDL_Event& _event);
    };

}


#endif //ENGINE2_0_WINDOW_INPUT_H
