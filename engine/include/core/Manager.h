//
// Created by borja on 17/05/22.
//

#ifndef ENGINE_MANAGER_H
#define ENGINE_MANAGER_H

#include "core/render/elements/TextureAtlasManager.h"
#include "core/render/window/input/Input.h"
#include "core/systems/console/Console.h"
#include "core/systems/uiSystem/FontManager.h"
#include "core/systems/soundSystem/SoundManager.h"
#include "core/systems/uiSystem/SceneManager.h"

namespace GDE {

    class Manager {
        public:
            Manager();

            TextureAtlasManager textureManager;
            ShaderManager shaderManager;
            FontManager fontManager;
            SoundManager soundManager;
            InputManager inputManager;
            Console consoleManager;
            SceneManager sceneManager;

        public:
            void init(Engine* _engine);
            void destroy();
    };

}

#endif //ENGINE_MANAGER_H
