//
// Created by borja on 16/05/22.
//

#ifndef GDE_SCENE_MANAGER_H
#define GDE_SCENE_MANAGER_H

#include "core/util/Util.h"
#include "core/systems/eventSystem/Event.h"

namespace GDE {

    class Scene;
    class Engine;

    /**
     * @attention All managers destroy all the resources before closing the application, it is not needed to be done by the end user.
     * @brief This class is in charge of loading and unloading scenes in memory and to decided when to render each one.
     */
    class SceneManager {
        private:
            /**
             * @brief Map SceneName -> Scene*.
             */
            std::unordered_map<std::string, Scene*> scenes;

            /**
             * @brief Current displaying scene.
             */
            Scene* sceneDisplayed = nullptr;

            /**
             * @see Engine
             */
            Engine* engine;

            /**
             * @brief This scene is used just in case end-user unloads the current scene without any other scene loaded.
             * @see Scene
             */
            Scene* defaultScene;

        public:
            /**
             * @attention This function should not be called by end users.
             * @brief Initiates the system.
             * @param _engine Engine
             */
            void init(Engine* _engine);

            /**
             * @brief Loads a scene in memory, but it not shows it yet.
             * @param _scene Scene to load
             * @param _sceneName Scene name
             */
            void loadScene(Scene* _scene, const std::string& _sceneName);

            /**
             * @brief Renders a scene that has been loaded previously.
             * @param _sceneName Scene name
             */
            void displayScene(const std::string& _sceneName);

            /**
             * @brief Returns an specific scene by name.
             * @param _sceneName Scene name
             * @return Scene*
             */
            Scene* getScene(const std::string& _sceneName);

            /**
             * @brief Returns the current displayed scene.
             * @return Scene*
             */
            Scene* getDisplayedScene();

            /**
             * @brief Unloads from memory an specific scene. If the displayed scene is unloaded, a default one will be displayed
             * until displayScene is called again on another loaded scene.
             * @param _sceneName Scene name
             */
            void unloadScene(const std::string& _sceneName);

            /**
             * @attention This function should not be called by end users.
             * @brief Destroys the system and all the loaded scenes.
             */
            void destroy();
    };

}

#endif //GDE_SCENE_MANAGER_H
