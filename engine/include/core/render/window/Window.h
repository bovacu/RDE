#pragma once

#include "core/util/Delegate.h"
#ifndef RDE_WINDOW_H
#define RDE_WINDOW_H

#include "core/util/Vec.h"
#include "core/systems/eventSystem/Event.h"
#include "core/platform/PlatformHeaderSDL.h"

namespace RDE {

    /**
     * @brief This class represents the window of the application and has methods to modify its components and get its data.
     */
    struct RDEConfig;

    class Window {

        friend class WindowInput;

        protected:
            /**
             * @brief SDL_Window used as a base of the engine Window.
             */
            SDL_Window* window = nullptr;

            /**
             * @brief Context used by SDL and OpenGL to render.
             */
            SDL_GLContext context {};

            /**
             * @brief Sets the function that will handle the events of the Window/
             * @see Engine::onEvent(Event&)
             */
            UniqueDelegate<void(Event&)> eventCallback;

            /**
             * @see RDEConfig
             */
            RDEConfig* properties = nullptr;

            /**
             * @brief If the Window is minimized or not.
             */
            bool minimized = false;

            /**
             * @brief If the Window has the focus.
             */
            bool hasFocus = true;

            /**
             * @brief If the window is running or not.
             */
            bool running = true;

            int currentDisplayIndex = -1;

        public:
            /**
             * @brief This callback lets the user define how and when the window should run the game. By default it is
             * set to run always, but typical customization could be only run if the window is focused or if the window
             * is not minimized.
             */
            UniqueDelegate<bool()> shouldUpdateWindow;

            /**
             * @brief Callback invoked each time the window is moved.
             */
            Delegate<void(const Vec2F&)> onWindowMovedCallback;

            /**
             * @brief Callback invoked each time the window is resized.
             */
            Delegate<void(const Vec2I&)> onWindowResizedCallback;

            /**
             * @brief Callback invoked each time the window is changed of display.
             */
            Delegate<void(int)> onWindowDisplayChangedCallback;

            /**
             * @brief Callback invoked each time the window is minimized.
             */
            Delegate<void()> onWindowMinimizedCallback;

            /**
             * @brief Callback invoked each time the window is maximized.
             */
            Delegate<void()> onWindowMaximizedCallback;

            /**
             * @brief Callback invoked each time the window is focused.
             */
            Delegate<void()> onWindowFocusedCallback;

            /**
             * @brief Callback invoked each time the window is unfocused.
             */
            Delegate<void()> onWindowUnfocusedCallback;

        protected:
            explicit Window(RDEConfig* _props);

        public:
            ~Window() ;

            /**
             * @brief This method is used to update the window, poll the events and
             * swap the buffers.
             */
            void update();

            /**
             * @brief Handles an specific event.
             * @param _e Event to be handled
             */
            void consumeEvent(Event& _e) const { eventCallback(_e); }

            /**
             * @brief Returns the width of the window.
             * @return int
             */
            [[nodiscard]] int getWidth() const;

            /**
             * @brief Returns the height of the window.
             * @return int
             */
            [[nodiscard]] int getHeight() const;

            /**
             * @brief Returns the title of the window.
             * @return std::string&
             */
            std::string& getTitle();

            /**
             * @brief Sets the new title of the window.
             * @param _title Title of the window.
             */
            void setTitle(const std::string& _title);

            /**
             * @brief Sets the new size of the window.
             * @param _width New width of the window.
             * @param _height New height of the window.
             */
            void setWindowSize(int _width, int _height) ;

            /**
             * @brief Returns the size of the window.
             * @return Vec2I
             */
            [[nodiscard]] Vec2I getWindowSize() const;

            /**
             * @brief Enables or disables fullscreen mode.
             * @param _fullscreen True or false to enable/disable.
             */
            void setFullscreen(bool _fullscreen);

            /**
             * @brief Returns if screen is fullscreen or not.
             * @return bool
             */
            [[nodiscard]] bool isFullscreen() const;

            /**
             * @brief Sets the method that will be executed when a polled event triggers.
             * @param _callback A void method with an Event& as parameter.
             */
            void setEventCallback(const UniqueDelegate<void(Event&)>& _callback);

            /**
             * @brief Enables or disables VSync.
             * @param _enable True/false to enable/disable.
             */
            void setVSync(bool _enable);

            /**
             * @brief Returns if the VSync is enabled.
             * @return bool
             */
            [[nodiscard]] bool isVSyncActive() const;

            /**
             * @brief Returns a raw pointer to one of the Window subclasses.
             * @return SDL_Window*
             */
            [[nodiscard]] SDL_Window* getNativeWindow() const;

            /**
             * @brief Returns the context of the Window.
             * @return SDL_GLContext&
             */
            [[nodiscard]] SDL_GLContext& getContext();

            /**
             * @brief Returns the position of the window.
             * @return Vec2I
             */
            [[nodiscard]] Vec2I getPosition() const;

            /**
             * @brief Sets the new position of the window.
             * @param _position Vec2I with the new [x,y]
             */
            void setPosition(const Vec2I& _position);

            /**
             * @brief Sets the icon of the window.
             * @param _path Path to the icon, it can be png, jpg, jpeg, ico...
             */
            void setIcon(const std::string& _path);

            /**
             * @brief Returns if the window is minimized or not.
             * @return bool
             */
            bool isMinimized();

            /**
             * @brief Returns if the Window is running or not.
             * @return bool
             */
            bool isRunning();

            /**
             * @brief Returns if the Window is focused or not.
             * @return bool
             */
            bool isFocused();

            /**
             * @brief Makes the window to close and the application to exit.
             */
            void stop();

            /**
             * @brief Refresh inner calculations for new DPIs on the screen.
             */
            void refreshDpi();

            /**
             * @brief Traps the mouse inside the window or allows it to move outside.
             * @param _allow If true, mouse will be able to leave the window, otherwise it won't.
             */
            void allowMouseToMoveOutOfWindow(bool _allow);
    };

}

#endif // RDE_WINDOW_H
