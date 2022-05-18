//
// Created by borja on 17/05/22.
//

#include "core/Manager.h"
#include "core/render/Renderer.h"
#include "core/Engine.h"

namespace GDE {

    Manager::Manager() {  }

    void Manager::init(Engine* _engine) {
        shaderManager.init();
        inputManager.init(_engine, &_engine->getWindow());
        fontManager.init();
        consoleManager.init();
        soundManager.init();
        sceneManager.init(this, &_engine->getWindow());
        Renderer::init(_engine);
        Physics::get().init();
    }

    void Manager::destroy() {
        shaderManager.destroy();
        inputManager.destroy();
        fontManager.destroy();
        soundManager.destroy();
        textureManager.destroy();
        sceneManager.destroy();
        Renderer::destroy();
        Physics::get().destroy();
    }
}