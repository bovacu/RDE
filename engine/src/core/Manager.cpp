//
// Created by borja on 17/05/22.
//

#include "core/Manager.h"
#include "core/render/Renderer.h"
#include "core/Engine.h"

namespace GDE {

    Manager::Manager() {  }

    void Manager::init(Engine* _engine) {
        shaderManager.init(&fileManager);
        Renderer::init(_engine);
        inputManager.init(_engine, &_engine->getWindow());
        fontManager.init();
        consoleManager.init();
        soundManager.init();
        sceneManager.init(_engine);
        physics.init();
    }

    void Manager::destroy() {
        shaderManager.destroy();
        inputManager.destroy();
        fontManager.destroy();
        soundManager.destroy();
        textureManager.destroy();
        sceneManager.destroy();
        Renderer::destroy();
        physics.destroy();
        ecsManager.destroy();
    }
}