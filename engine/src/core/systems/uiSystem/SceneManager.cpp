//
// Created by borja on 16/05/22.
//

#include "core/systems/uiSystem/SceneManager.h"
#include "core/systems/configSystem/ConfigManager.h"
#include "core/Engine.h"

namespace GDE {

    const std::string SCENES_PATH = "assets/scenes/";

    void SceneManager::init(Manager* _manager, Window* _window) {
        manager = _manager;
        window = _window;
    }

    Scene* SceneManager::getDisplayedScene() {
        return sceneDisplayed;
    }

    void SceneManager::addScene(Scene* _scene, const std::string& _sceneName) {
        scenes[_sceneName] = _scene;
        ConfigManager::loadScene(manager, _scene, window, APPEND_S(SCENES_PATH, _sceneName, ".yaml"));
    }

    void SceneManager::displayScene(const std::string& _sceneName) {
        scenes[_sceneName]->onInit();
        sceneDisplayed = scenes[_sceneName];
    }

    Scene* SceneManager::getScene(const std::string& _sceneName) {
        return scenes[_sceneName];
    }

    void SceneManager::unloadScene(const std::string& _sceneName) {
        auto* _scene = scenes[_sceneName];
        _scene->onEnd();
        delete _scene;
        scenes.erase(_sceneName);
    }

    void SceneManager::destroy() {
        for(auto& _scene : scenes) {
            _scene.second->onEnd();
            delete _scene.second;
        };

        scenes.clear();
    }
}