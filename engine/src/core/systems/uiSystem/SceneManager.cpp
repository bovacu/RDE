//
// Created by borja on 16/05/22.
//

#include "core/systems/uiSystem/SceneManager.h"
#include "core/systems/configSystem/ConfigManager.h"
#include "core/Engine.h"

namespace GDE {

    struct DefaultScene : public Scene {

        public:
            DefaultScene(Engine* _engine, const std::string& _name) : Scene(_engine, _name) { }
    };

    const std::string SCENES_PATH = "assets/scenes/";

    void SceneManager::init(Engine* _engine) {
        engine = _engine;
        defaultScene = new DefaultScene(_engine, "__default__");
    }

    Scene* SceneManager::getDisplayedScene() {
        return sceneDisplayed;
    }

    void SceneManager::loadScene(Scene* _scene, const std::string& _sceneName) {
        scenes[_sceneName] = _scene;
        ConfigManager::loadScene(&engine->manager, _scene, &engine->getWindow(), APPEND_S(SCENES_PATH, _sceneName, ".yaml"));
    }

    void SceneManager::displayScene(const std::string& _sceneName) {
        scenes[_sceneName]->onInit();
        sceneDisplayed = scenes[_sceneName];
    }

    Scene* SceneManager::getScene(const std::string& _sceneName) {
        return scenes[_sceneName];
    }

    void SceneManager::unloadScene(const std::string& _sceneName) {
        if(sceneDisplayed == defaultScene) return;

        bool _deletingDefaultScene = false;
        auto* _scene = scenes[_sceneName];
        _scene->onEnd();
        if(_scene == sceneDisplayed) _deletingDefaultScene = true;
        delete _scene;
        scenes.erase(_sceneName);

        if(_deletingDefaultScene) sceneDisplayed = defaultScene;
    }

    void SceneManager::destroy() {
        for(auto& _scene : scenes) {
            _scene.second->onEnd();
            delete _scene.second;
        };

        scenes.clear();
        delete defaultScene;
    }
}