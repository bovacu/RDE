// Created by borja on 24/12/21.


#ifndef ENGINE2_0_ENGINE_H
#define ENGINE2_0_ENGINE_H


#include "core/render/window/Window.h"
#include "core/render/window/event/WindowEvent.h"
#include "core/systems/profiling/Profiler.h"
#include "core/graph/ImGuiScene.h"
#include "core/render/elements/FrameBuffer.h"
#include "core/Manager.h"


namespace GDE {

    class Scene;
    struct GDEConfig {
        struct WindowProperties {
            std::string title = "Default";
            Vec2<unsigned int> size { 1280, 720 };
            bool fullScreen = false;
            bool vsync = true;
        };

        struct ProjectProperties {
            std::string iconPath;
            std::string mainSceneToLoad;
        };

        WindowProperties windowData;
        ProjectProperties projectData;
        std::unordered_map<std::string, Scene*> registeredScenes;
    };

    class Engine {
        public:
            Manager manager;

        private:
            GDEConfig gdeConfig;

            float fixedDelta = -1;
            float timer = 0;
            Delta dt;
            unsigned int fpsCounter = 0, frameCounter = 0;
            Color backgroundColor = Color::Red;
            FrameBuffer* frameBuffer;

            bool running = true;
            bool minimized = false;
            bool showImGuiDebugWindow = false;

            Window* window;
            static Engine* gameInstance;

            #if !IS_MOBILE()
            ImGuiScene* imGuiLayer;
            #endif

            UDelegate<bool(WindowResizedEvent&)> wreDel;

        private:
            void updateFps();

        public:
            Engine();

            void setMinimized(bool _isMinimized) { minimized = _isMinimized; }
            [[nodiscard]] bool isMinimized() const { return minimized; }

            /// This method provides the number of FPS in any given moment.
            /// @return the number of FPS
            [[nodiscard]] int getFps() const;

            /// Sets the title of the game's window.
            /// @param [in] _title The title of the window
            void setTitle(const std::string& _title);

            /// Gives the title of the window.
            /// @return the title of the window.
            [[nodiscard]] const std::string& getTitle() const       { return window->getTitle(); }

            /// Sets the size of the window.
            /// @param _width Width of the window.
            /// @param _height Height of the window.
            void setWindowSize(int _width, int _height);

            /// Returns the size of the window as a Vec2i.
            ///@return Vec2i with the size of the window.
            [[nodiscard]] Vec2I getWindowSize() const { return Vec2I(window->getWidth(), window->getHeight()); }

            /// Returns 1 / FPS
            /// @return The FPS as milliseconds.
            [[nodiscard]] float geFixedDelta() const { return fixedDelta; }
            void setFixedDelta(float _fixedDelta) { fixedDelta = _fixedDelta; }

            /// Enables or disables the VSync
            /// @param true or false depending if you want to enable(true) or disable(false).
            void setVSync(bool _vsync);

            /// Returns if the VSync is active.
            /// @return If VSync is active.
            bool isVSync();

            /// Returns the delta (time passed between the actual frame and the previous one) on any moment.
            /// @return The current delta.
            [[nodiscard]] Delta getDelta() const { return dt; }

            /// Enables or disables the fullscreen mode.
            /// @param _fullscreen Enables(true) or disables(false) the fullscreen.
            void setFullscreen(bool _fullscreen);

            /// Returns if fullscreen is active.
            /// @return If fullscreen is active.
            [[nodiscard]] bool isFullscreen() const {  return window->isFullscreen(); }

            /// Sets the position of the window on the screen.
            /// @param _position Vec2i with [x,y] coordinates to move the window.
            void setPosition(const Vec2I& _position) { window->setPosition(_position); }

            /// Returns a Vec2i with the current position of the window.
            /// @return Vec2i with the position of the window.
            [[nodiscard]] Vec2I getPosition() const { return window->getPosition(); }

            /// Allows you to set the icon of the window.
            /// @param _path Path to the image to be used as icon, can be in png, jpg, jpeg, ico...
            void setAppIcon(const char* _path) { window->setIcon(_path); }

            void setRunning(bool _running) { running = _running; }

        public:
            void onInit(Scene* _scene);
            /// This method is the main loop of the game, runs until the window is closed or running attribute is set
            /// to false. Runs with an independent frame rate loop.
            void onRun();

            /// This methods handles all the events that can't be handle in any given moment, such as resizing the
            /// window, closing the window, moving it...
            /// @param The Event to be handled
            /// @sa Event
            void onEvent(Event& _event);

            /// This method handles the non-physics based updates of the game and also is used to capture all the inputs.
            /// @param _dt The amount of time passed from the previous frame to this current one.
            void onUpdate(Delta _dt);

            /// This method is like onUpdate, but this one handles the physics-based updates.
            /// @param _fixedDt Fixed value to multiply the physics movements, by default 1 / 60.
            void onFixedUpdate(Delta _fixedDt);

            /// This method is used to render everything in our game, renderization shouldn't be done other method.
            /// @param _dt The amount of time passed from the previous frame to this current one.
            void onRender(Delta _dt);

            void destroy();

            /// Returns the Window instance.
            /// @return The main window.
            Window& getWindow() { return *window; }

            /// Returns a reference to the application, as it is created as a Singleton.
            /// @return A reference to the application.
            static Engine& get() { return *Engine::gameInstance; }

        private:
            bool onWindowClosed(WindowClosedEvent& _e);
            bool onWindowResized(WindowResizedEvent& _e);

            Logs changeColorConsoleCommand(const std::vector<std::string>& _args);
            Logs setParentCommand(const std::vector<std::string>& _args);
            Logs componentsCommands(const std::vector<std::string>& _args);
    };
}


#endif //ENGINE2_0_ENGINE_H
