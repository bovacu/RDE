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
#include "core/render/elements/ShaderManager.h"
#include "core/systems/ecsSystem/ECSManager.h"

namespace GDE {

    /// This class contains all of the managers of the engine, so end users can access all of them from one single place.
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
            ECSManager ecsManager;

        public:
            /// This function should not be called by end users.
            void init(Engine* _engine);
            /// This function should not be called by end users.
            void destroy();
    };

}

#endif //ENGINE_MANAGER_H
