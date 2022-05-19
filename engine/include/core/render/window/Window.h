#pragma once

#ifndef WINDOW_H
#define WINDOW_H

#include "core/util/Util.h"
#include "core/render/window/event/Event.h"
#include "core/platform/PlatformHeaderSDL.h"

namespace GDE {

    /// This class represents the window of the application and has methods to modify its components and get its data.
    struct GDEConfig;
    class Window {
        friend class WindowInput;

        private:
            SDL_Window* window;
            SDL_GLContext context {};
            UDelegate<void(Event&)> eventCallback;
            GDEConfig* properties;
            bool minimized = false;
            bool running = true;

        public:
            explicit Window(GDEConfig* _props);
            ~Window() ;

            /// This method is used to update the window, poll the events and
            /// swap the buffers.
            void update();

            void consumeEvent(Event& _e) const { eventCallback(_e); }

            /// Returns the width of the window.
            /// @return width of the window.
            [[nodiscard]] int getWidth() const;

            /// Returns the height of the window.
            /// @return height of the window.
            [[nodiscard]] int getHeight() const;

            /// Returns the title of the window.
            /// @return the title of the window as a string.
            std::string& getTitle();

            /// Sets the new title of the window.
            /// @param _title Title of the window.
            void setTitle(const std::string& _title);

            /// Sets the new size of the window.
            /// @param _width new width of the window.
            /// @param _height new height of the window.
            void setWindowSize(int _width, int _height) ;

            /// Returns the size of the window.
            /// @return Vec2i with the [width,height] of the window.
            [[nodiscard]] Vec2I getWindowSize() const;

            /// Enables or disables fullscreen mode.
            /// @param _fullscreen true or false to enable/disable.
            void setFullscreen(bool _fullscreen);

            /// Returns if screen is fullscreen or not.
            /// @return true if is fullscreen, false otherwise.
            [[nodiscard]] bool isFullscreen() const;

            /// Sets the method that will be executed when a polled event triggers.
            /// @param _callback a void method with an Event& as parameter.
            void setEventCallback(const UDelegate<void(Event&)>& _callback);

            /// Enables or disables VSync.
            /// @param _enable true/false to enable/disable.
            void setVSync(bool _enable);

            /// Returns if the VSync is enabled.
            /// @return true if is enabled, otherwise false.
            [[nodiscard]] bool isVSyncActive() const;

            /// Returns a raw pointer to one of the Window subclasses.
            /// @return raw pointer to WindowsWindow, LinuxWindow or MacWindow.
            [[nodiscard]] SDL_Window* getNativeWindow() const;

            [[nodiscard]] SDL_GLContext& getContext();

            /// Returns the position of the window.
            /// @return Vec2i with the [x,y] of the window.
            [[nodiscard]] Vec2I getPosition() const;

            /// Sets the new position of the window.
            /// @param _position Vec2i with the new [x,y].
            void setPosition(const Vec2I& _position);

            /// Sets the icon of the window.
            /// @param _path path to the icon, it can be png, jpg, jpeg, ico...
            void setIcon(const std::string& _path);

            /// @return if the window is minimized or not, on mobile it also triggers being on second plane
            bool isMinimized();

            /// @return if the application is still running
            bool isRunning();

            /// Makes the window to close and the application to exit
            void stop();

            void swapBuffers() const;
    };

}

#endif // WINDOW_H
