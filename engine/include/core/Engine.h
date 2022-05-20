// Created by borja on 24/12/21.


#ifndef ENGINE2_0_ENGINE_H
#define ENGINE2_0_ENGINE_H


#include "core/render/window/Window.h"
#include "core/render/window/event/WindowEvent.h"
#include "core/graph/ImGuiScene.h"
#include "core/render/elements/FrameBuffer.h"
#include "core/Manager.h"
#include "core/systems/configSystem/ConfigManager.h"

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
            GDEConfig gdeConfig;

        private:
            float fixedDelta = -1;
            float timer = 0;
            unsigned int fpsCounter = 0, frameCounter = 0;
            Color backgroundColor = Color::Red;
            FrameBuffer* frameBuffer;

            Window* window;

            #if !IS_MOBILE()
            ImGuiScene* imGuiLayer;
            #endif

            UDelegate<bool(WindowResizedEvent&)> wreDel;

        private:
            void updateFps();

        public:
            Engine();

            /// This method provides the number of FPS in any given moment.
            /// @return the number of FPS
            [[nodiscard]] int getFps() const;

            /// Returns the delta used in the FixedUpdate method
            /// @return The FPS as milliseconds.
            [[nodiscard]] float geFixedDelta() const;

            /// Sets the delta to use in the FixedUpdate method, usually 1/60, 1/50 or 1/30
            void setFixedDelta(float _fixedDelta);

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

            /// Frees all memory and resources allocated
            void destroy();

            /// Returns the Window instance.
            /// @return The main window.
            Window& getWindow() { return *window; }

        private:
            bool onWindowResized(WindowResizedEvent& _e);

            Logs changeColorConsoleCommand(const std::vector<std::string>& _args);
            Logs setParentCommand(const std::vector<std::string>& _args);
            Logs componentsCommands(const std::vector<std::string>& _args);
    };
}


#endif //ENGINE2_0_ENGINE_H
