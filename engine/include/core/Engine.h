// Created by borja on 24/12/21.


#ifndef RDE_ENGINE_H
#define RDE_ENGINE_H

#include "core/systems/eventSystem/WindowEvent.h"
#include "core/Manager.h"
#include "core/platform/Platform.h"
#include "core/DeviceInfoProvider.h"

// int main(int argc, char** argv);

namespace RDE {

    class Scene;
    class FrameBuffer;
    class ImGuiScene;
    class Window;

    struct LocalizationConfig {
        /**
         * @brief Path to the file localization.json, if it exists
         */
        std::string localizationPath;

        /**
         * @brief Symbol to use to replace arguments. By default is ~
         */
         std::string replacementSymbol = "~";
    };

    /**
     * @brief This struct contains some basic configuration of the window.
     */
    struct RDEConfig {
        struct WindowProperties {
            /**
             * @brief Title of the window.
             */
            std::string title = "Default";

            /**
             * @brief Size of the window.
             */
            Vec2I size { 1280, 720 };

            Vec2I resolution { 1280, 720 };

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

            float verticalDpi = 0, horizontalDpi = 0, diagonalDpi = 0;
        };

        /**
         * @brief Specific data of the project itself.
         */
        struct ProjectProperties {
            /**
             * @brief Icon to be shown in the task bar and in the window's icon.
             */
            std::string iconPath;

            /**
             * @brief Path to the file resources.json, if it exists
             */
            std::string resourcesPath;

            /**
             * @brief Configuration for localization.
             */
            LocalizationConfig localizationConfig;
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
        friend class Window;

        public:
            /**
             * @see Manager
             */
            Manager manager {};

            /**
             * @see RDEConfig
             */
            RDEConfig rdeConfig {};

            /**
             * @see DeviceInfoProvider
            */
            DeviceInfoProvider devideInfoProvider {};

            /**
            * @see FrameBuffer
            */
            FrameBuffer* mainFrameBuffer = nullptr;

        private:
            /**
             * @brief Delta time of the Fixed Update method.
             */
            float fixedDelta = -1;

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
             * @brief Function to render in a custom way.
             */
            UniqueDelegate<void(FrameBuffer*)> redirectionFunc;

            /**
            * @brief Function to render to ImGui.
            */
            UniqueDelegate<void(FrameBuffer*)> imGuiRedirectionFunc;

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
            UniqueDelegate<bool(WindowResizedEvent&)> wreDel;

            Platform platform;

            Delta dt {};
            int accumulator = 0;

        public:
            RDE_FUNC Engine();

            /**
             * @brief Returns the platform that the engine is running on.
             * @return RDE_PLATFORM_TYPE_
             */
            RDE_FUNC RDE_PLATFORM_TYPE_ getPlatform() const;

            /**
             * @brief This method provides the number of FPS in any given moment.
             * @return the number of FPS
             */
            RDE_FUNC_ND int getFps() const;

            /**
             * @brief Returns the delta used in the FixedUpdate method
             * @return The FPS as milliseconds.
             */
            RDE_FUNC_ND float geFixedDelta() const;

            /**
             * @brief Sets the delta to use in the FixedUpdate method, usually 1/60, 1/50 or 1/30
             */
            RDE_FUNC void setFixedDelta(float _fixedDelta);

            /**
             * @brief This initiates some crucial parts of the engine and also the main scene.
             * @param _scene Main scene
             * @see Scene
             */
            RDE_FUNC void onInit(Scene* _scene);

            /**
             * @brief This method is the main loop of the game, runs until the window is closed or running attribute is set
             * to false. Runs with an independent frame rate loop.
             */
            RDE_FUNC void onRun();

            /**
             * @brief This methods handles all the events that can't be handle in any given moment, such as resizing the
             * window, closing the window, moving it...
             * @param The Event to be handled
             * @see Event
             */
            RDE_FUNC void onEvent(Event& _event);

            /**
             * @brief This method handles the non-physics based updates of the game and also is used to capture all the inputs.
             * @param _dt The amount of time passed from the previous frame to this current one.
             */
            RDE_FUNC void onUpdate(Delta _dt);

            /**
             * @brief This method is like onUpdate, but this one handles the physics-based updates.
             * @param _fixedDt Fixed value to multiply the physics movements, by default 1 / 60.
             */
            RDE_FUNC void onFixedUpdate(Delta _fixedDt);

            /**
             * @brief This method is called once onUpdate and onFixedUpdate are finished, but before onRender. This is
             * the perfect place to remove elements from collections used in onUpdate or onFixedUpdate.
             * @param _dt The amount of time passed from the previous frame to this current one.
             */
            RDE_FUNC void onLateUpdate(Delta _dt);

            /**
             * @brief This method is used to render everything in our game, renderization shouldn't be done other method.
             * @param _dt The amount of time passed from the previous frame to this current one.
             */
            RDE_FUNC void onRender(Delta _dt);

            /**
             * @brief Frees all memory and resources allocated
             */
            RDE_FUNC void destroy();

            /**
             * @brief Returns the Window instance.
             * @return The main window.
             */
            RDE_FUNC Window* getWindow() { return window; }

            /**
             * @brief Allows the end-user to render to something different than the default window.
             * @param _redirectionFunc FrameBuffer
             */
            RDE_FUNC void setRenderingRedirection(UniqueDelegate<void(FrameBuffer*)>& _redirectionFunc);

            /**
             * @brief Allows the end-user to render to ImGui specifically.
             * @param _redirectionFunc FrameBuffer
             */
            RDE_FUNC void setRenderingRedirectionToImGui(UniqueDelegate<void(FrameBuffer*)>& _redirectionFunc);

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

            /**
             * @brief This function will contain the code of the inner main loop without the while, so it can also be passed to Emscripten 
             */
            void loopCode();
    };

    /**
     * @brief Externed function that must be implemented for the Engine to work.
     */
    RDE_FUNC_ENGINE Engine* createEngine(int _argc, char** _argv);
}


#endif //RDE_ENGINE_H
