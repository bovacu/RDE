//
// Created by borja on 17/05/22.
//

#include "core/Manager.h"
#include "core/render/Renderer.h"

namespace GDE {

    Manager::Manager() {  }

    void Manager::init(Window* _window) {
        shaderManager.init();
        inputManager.init(_window);
        fontManager.init();
        consoleManager.init();
        soundManager.init();
        sceneManager.init(this);
        Renderer::init(this);
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