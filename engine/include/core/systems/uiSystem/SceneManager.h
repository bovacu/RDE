//
// Created by borja on 16/05/22.
//

#ifndef ENGINE_SCENE_MANAGER_H
#define ENGINE_SCENE_MANAGER_H

#include "core/util/Util.h"
#include "core/render/window/event/Event.h"

namespace GDE {

    class Scene;
    class Manager;
    class SceneManager {
        private:
            std::unordered_map<std::string, Scene*> scenes;
            Scene* sceneDisplayed = nullptr;
            Manager* manager;

        public:
            void init(Manager* _manager);
            void addScene(Scene* _scene, const std::string& _sceneName);
            void displayScene(const std::string& _sceneName);
            Scene* getScene(const std::string& _sceneName);
            Scene* getDisplayedScene();

            void unloadScene(const std::string& _sceneName);
            void destroy();
    };

}

#endif //ENGINE_SCENE_MANAGER_H
