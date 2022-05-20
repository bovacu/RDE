//
// Created by borja on 16/05/22.
//

#ifndef ENGINE_SCENE_MANAGER_H
#define ENGINE_SCENE_MANAGER_H

#include "core/util/Util.h"
#include "core/render/window/event/Event.h"

namespace GDE {

    class Scene;
    class Engine;

    /// This class is in charge of loading and unloading scenes in memory and to decided when to render each one.
    /// All managers destroy all the resources before closing the application, it is not needed to be done by the end user.
    class SceneManager {
        private:
            std::unordered_map<std::string, Scene*> scenes;
            Scene* sceneDisplayed = nullptr;
            Engine* engine;
            Scene* defaultScene;

        public:
            /// This function should not be called by end users.
            void init(Engine* _engine);

            /// Loads a scene in memory, but it not shows it yet.
            void loadScene(Scene* _scene, const std::string& _sceneName);

            /// Renders a scene that has been loaded previously.
            void displayScene(const std::string& _sceneName);

            /// Returns an specific scene by name.
            Scene* getScene(const std::string& _sceneName);

            /// Returns the current displayed scene.
            Scene* getDisplayedScene();

            /// Unloads from memory an specific scene. If the displayed scene is unloaded, a default one will be displayed
            /// until displayScene is called again on another loaded scene.
            void unloadScene(const std::string& _sceneName);

            /// This function should not be called by end users.
            void destroy();
    };

}

#endif //ENGINE_SCENE_MANAGER_H
