#pragma once

#ifndef WINDOW_H
#define WINDOW_H

#include "core/util/Util.h"
#include "core/render/window/event/Event.h"
#include "core/platform/PlatformHeaderSDL.h"

namespace GDE {

    /// Method that will be invoked when polling the events.
    using EventCallbackFn = std::function<void (Event&)>;
    
    class Window;
    typedef std::unique_ptr<Window> WindowPtr;

    /// Options of the window that can be customize. It works as a flag system, like ImGui.
    typedef int WindowOptions;
    enum WindowOptions_ : unsigned {    WindowOptions_Minimize    = 1u << 0u, /// 00000001
                                        WindowOptions_Maximize    = 1u << 1u, /// 00000010
                                        WindowOptions_Resize      = 1u << 2u, /// 00000100
                                        WindowOptions_Visible     = 1u << 3u, /// 00001000
                                        WindowOptions_H_Scroll    = 1u << 4u, /// 00010000
                                        WindowOptions_V_Scroll    = 1u << 5u, /// 00100000
                                        WindowOptions_Disabled    = 1u << 6u  /// 01000000
    };

    /// Group of basic data of the window.
    struct WindowProperties {
        std::string title;
        unsigned int width;
        unsigned int height;

        Vec2I position;

        explicit WindowProperties(std::string _title = "Engine: ", unsigned int _width = 1280, unsigned int _height = 720) :
                title(std::move(_title)), width(_width), height(_height) {  }
    };

    /// This class represents the window of the application and has methods to modify its components and get its data.
    class Window {
        private:
            SDL_Window* window;
            SDL_GLContext context;

            struct WindowData {
                std::string title;
                int width, height;
                bool vSync;
                bool fullscreen;
                Vec2I position;

                UDelegate<void(Event&)> eventCallback;
            };

            WindowData data;

        public:
            explicit Window(const WindowProperties& _props);
            ~Window() ;

            /// This method is used to update the window, poll the events and
            /// swap the buffers.
            void update();

            void consumeEvent(Event& _e) const { data.eventCallback(_e); }

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

            void setWindowSizeAndUpdateNativeWindow(int _width, int _height);

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
            void setIcon(const char* _path);

            /// Sets the window options.
            /// @param _op options of the window, such as resizing, maximize/minimize button...
            /// @param _allow true/false to enable/disable.
            void setWindowOptions(WindowOptions _op, bool _allow);

            void swapBuffers() const;

            static std::unique_ptr<Window> createWindow(const WindowProperties& _windowProperties = WindowProperties());

        private:
            void init(const WindowProperties& _props);
            void shutdown();

            /// Method to transformMatrix from engine data to GLFW data. Will be only used if using GLFW.
            long windowOptionsToGLFW(WindowOptions _options, bool _allow);
    };

}

#endif // WINDOW_H
