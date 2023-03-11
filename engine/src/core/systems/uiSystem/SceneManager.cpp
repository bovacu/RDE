//
// Created by borja on 16/05/22.
//

#include "core/systems/uiSystem/SceneManager.h"
#include "core/systems/configSystem/ConfigManager.h"
#include "core/Engine.h"
#include "core/graph/Scene.h"
#include "core/systems/uiSystem/Canvas.h"

namespace RDE {

    struct DefaultScene : public Scene {
        public:
            DefaultScene(Engine* _engine, const std::string& _name) : Scene(_engine, _name) { }
            ~DefaultScene() {  }
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
        if(scenes.find(_sceneName) != scenes.end()) {
            Util::Log::warn("Scene '", _sceneName, "' was already loaded, so no loading was made");
            return;
        }
        scenes[_sceneName] = _scene;
        engine->manager.configManager.loadScene(&engine->manager, _scene, engine->getWindow(), Util::String::appendToString(SCENES_PATH, _sceneName, ".json"));
        Util::Log::debug("Loaded scene '", _sceneName, "'");
    }

    void SceneManager::displayScene(const std::string& _sceneName) {
        sceneDisplayed = scenes[_sceneName];
        sceneDisplayed->onInit();
        auto _windowSize = engine->getWindow()->getWindowSize();

        sceneDisplayed->mainCamera->onResize(_windowSize.x, _windowSize.y);
        sceneDisplayed->canvas->getCamera()->onResize(_windowSize.x, _windowSize.y);

        Util::Log::debug("Displayed scene '", _sceneName, "'");
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
        engine->manager.configManager.unloadScene(&engine->manager, _scene, Util::String::appendToString(SCENES_PATH, _sceneName, ".json"));
        delete _scene;
        scenes.erase(_sceneName);

        if(_deletingDefaultScene) sceneDisplayed = defaultScene;
        Util::Log::debug("Unloaded scene '", _sceneName, "'");
    }

    void SceneManager::destroy() {
        Util::Log::debug("Cleaning up SceneManager");
        for(auto& _scene : scenes) {
            _scene.second->onEnd();
            delete _scene.second;
        };

        scenes.clear();
        delete defaultScene;
    }
}