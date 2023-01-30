//
// Created by borja on 17/05/22.
//

#include "core/Manager.h"
#include "core/render/RenderManager.h"
#include "core/Engine.h"

namespace RDE {

    Manager::Manager() {  }

    void Manager::init(Engine* _engine) {
        engine = _engine;
        shaderManager.init(&fileManager);
        renderManager.init(_engine);
        inputManager.init(_engine, _engine->getWindow());
        fontManager.init(&fileManager);
        consoleManager.init();
        soundManager.init();
        sceneManager.init(_engine);
        physics.init();
        textureManager.init(&fileManager);
        localizationManager.init(_engine);
    }

    void Manager::destroy() {
        shaderManager.destroy();
        inputManager.destroy();
        fontManager.destroy();
        soundManager.destroy();
        textureManager.destroy();
        sceneManager.destroy();
        renderManager.destroy();
        physics.destroy();
        ecsManager.destroy();
    }
}