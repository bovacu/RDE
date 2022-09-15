// Created by borja on 24/12/21.


#ifndef ENGINE2_0_ENGINE_H
#define ENGINE2_0_ENGINE_H


#include "core/render/window/Window.h"
#include "core/systems/eventSystem/WindowEvent.h"
#include "core/graph/ImGuiScene.h"
#include "core/render/elements/FrameBuffer.h"
#include "core/Manager.h"
#include "core/systems/configSystem/ConfigManager.h"
#include "core/platform/Platform.h"

int main(int argc, char** argv);

namespace GDE {

    class Scene;

    /**
     * @brief This struct contains some basic configuration of the window.
     */
    struct GDEConfig {
        struct WindowProperties {
            /**
             * @brief Title of the window.
             */
            std::string title = "Default";

            /**
             * @brief Size of the window.
             */
            Vec2<unsigned int> size { 1280, 720 };

            /**
             * @brief Fullscreen active or not.
             */
            bool fullScreen = false;

            /**
             * @brief VSync active or not.
             */
            bool vsync = true;

            /**
             * @brief Allows the game to run even if it is not focused.
             */
            bool allowRunningOnBackground = true;
        };

        /**
         * @brief Specific data of the project itself.
         */
        struct ProjectProperties {
            /**
             * @brief Icon to be shown in the task bar and in the window's icon.
             */
            std::string iconPath = "";

            /**
             * @brief Name of the main scene to load.
             */
            std::string mainSceneToLoad = "";
        };

        /**
         * @see WindowProperties
         */
        WindowProperties windowData {};

        /**
         * @see ProjectProperties
         */
        ProjectProperties projectData {};

        /**
         * @brief All the scenes that are registered by the engine. Might or might not be loaded in memory.
         */
        std::unordered_map<std::string, Scene*> registeredScenes = {};
    };

    class Engine {
        public:
            /**
             * @see Manager
             */
            Manager manager {};

            /**
             * @see GDEConfig
             */
            GDEConfig gdeConfig {};

        private:
            /**
             * @brief Delta time of the Fixed Update method.
             */
            float fixedDelta = -1;

            /**
             * @brief Second counter for FPS.
             */
            float timer = 0;

            /**
             * @brief FPS counter.
             */
            unsigned int fpsCounter = 0;

            /**
             * @brief Frame counter.
             */
            unsigned int frameCounter = 0;

            /**
             * @brief Color of the background.
             */
            Color backgroundColor = Color::Red;

            /**
             * @see FrameBuffer
             */
            FrameBuffer* frameBuffer = nullptr;

            /**
             * @brief Function to render in a custom way.
             */
            UDelegate<void(FrameBuffer*)> redirectionFunc;

            /**
            * @brief Function to render to ImGui.
            */
            UDelegate<void(FrameBuffer*)> imGuiRedirectionFunc;

            /**
             * @see Window
             */
            Window* window = nullptr;

            #if !IS_MOBILE()
            /**
             * @brief ImGui Scene to show and do debugging operations.
             * @see ImGuiScene
             */
            ImGuiScene* imGuiLayer = nullptr;
            #endif

            /**
             * @brief Callback for window resizing.
             */
            UDelegate<bool(WindowResizedEvent&)> wreDel;

            Platform platform;

        public:
            Engine();

            /**
             * @brief This method provides the number of FPS in any given moment.
             * @return the number of FPS
             */
            [[nodiscard]] int getFps() const;

            /**
             * @brief Returns the delta used in the FixedUpdate method
             * @return The FPS as milliseconds.
             */
            [[nodiscard]] float geFixedDelta() const;

            /**
             * @brief Sets the delta to use in the FixedUpdate method, usually 1/60, 1/50 or 1/30
             */
            void setFixedDelta(float _fixedDelta);

            /**
             * @brief This initiates some crucial parts of the engine and also the main scene.
             * @param _scene Main scene
             * @see Scene
             */
            void onInit(Scene* _scene);

            /**
             * @brief This method is the main loop of the game, runs until the window is closed or running attribute is set
             * to false. Runs with an independent frame rate loop.
             */
            void onRun();

            /**
             * @brief This methods handles all the events that can't be handle in any given moment, such as resizing the
             * window, closing the window, moving it...
             * @param The Event to be handled
             * @see Event
             */
            void onEvent(Event& _event);

            /**
             * @brief This method handles the non-physics based updates of the game and also is used to capture all the inputs.
             * @param _dt The amount of time passed from the previous frame to this current one.
             */
            void onUpdate(Delta _dt);

            /**
             * @brief This method is like onUpdate, but this one handles the physics-based updates.
             * @param _fixedDt Fixed value to multiply the physics movements, by default 1 / 60.
             */
            void onFixedUpdate(Delta _fixedDt);

            /**
             * @brief This method is used to render everything in our game, renderization shouldn't be done other method.
             * @param _dt The amount of time passed from the previous frame to this current one.
             */
            void onRender(Delta _dt);

            /**
             * @brief Frees all memory and resources allocated
             */
            void destroy();

            /**
             * @brief Returns the Window instance.
             * @return The main window.
             */
            Window& getWindow() { return *window; }

            /**
             * @brief Allows the end-user to render to something different than the default window.
             * @param _redirectionFunc FrameBuffer
             */
            void setRenderingRedirection(UDelegate<void(FrameBuffer*)>& _redirectionFunc);

            /**
             * @brief Allows the end-user to render to ImGui specifically.
             * @param _redirectionFunc FrameBuffer
             */
            void setRenderingRedirectionToImGui(UDelegate<void(FrameBuffer*)>& _redirectionFunc);

        private:
            /**
             * @brief Callback to execute when the window is resized. Returns if the event was handled or if it should be
             * treated in deeper layers.
             * @param _e Event of resizing.
             * @return bool
             */
            bool onWindowResized(WindowResizedEvent& _e);

            /**
             * @brief Command for the internal console.
             * @param _args Arguments
             * @return Logs
             */
            Logs changeColorConsoleCommand(const std::vector<std::string>& _args);

            /**
             * @brief Command for the internal console.
             * @param _args Arguments
             * @return Logs
             */
            Logs setParentCommand(const std::vector<std::string>& _args);

            /**
             * @brief Command for the internal console.
             * @param _args Arguments
             * @return Logs
             */
            Logs componentsCommands(const std::vector<std::string>& _args);
    };

    Engine* createEngine(int _argc, char** _argv);
}


#endif //ENGINE2_0_ENGINE_H
