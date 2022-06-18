//
// Created by borja on 17/05/22.
//

#ifndef ENGINE_MANAGER_H
#define ENGINE_MANAGER_H

#include "core/render/elements/TextureAtlasManager.h"
#include "core/systems/inputSystem/input/Input.h"
#include "core/systems/console/Console.h"
#include "core/systems/uiSystem/FontManager.h"
#include "core/systems/soundSystem/SoundManager.h"
#include "core/systems/uiSystem/SceneManager.h"
#include "core/render/elements/ShaderManager.h"
#include "core/systems/ecsSystem/ECSManager.h"
#include "core/systems/physicsSystem/PhysicsManager.h"
#include "core/systems/configSystem/ConfigManager.h"

namespace GDE {

    /**
     * @brief This class contains all of the managers of the engine, so end users can access all of them from one single place.
     */
    class Manager {
        public:
            Manager();

            ConfigManager configManager;
            TextureAtlasManager textureManager;
            ShaderManager shaderManager;
            FontManager fontManager;
            SoundManager soundManager;
            InputManager inputManager;
            Console consoleManager;
            SceneManager sceneManager;
            ECSManager ecsManager;
            PhysicsManager physics;

        public:
            /**
             * @attention This function should not be called by end users.
             * @brief Initiates all of the managers.
             * @param _engine Reference to the engine.
             */
            void init(Engine* _engine);

            /**
             * @attention This function should not be called by end users.
             * @brief Destroys all of the managers.
             */
            void destroy();
    };

}

#endif //ENGINE_MANAGER_H
