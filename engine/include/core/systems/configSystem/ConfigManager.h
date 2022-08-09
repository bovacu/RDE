//
// Created by borja on 15/05/22.
//

#ifndef ENGINE_CONFIG_LOADER_H
#define ENGINE_CONFIG_LOADER_H

#include "nlohmann/json.hpp"
#include "core/graph/Scene.h"
#include "core/systems/fileSystem/FileManager.h"

namespace GDE {

    class Window;
    struct GDEConfig;

    /**
     * @brief This class is in charge of generating the Scenes and set up the initial configurations of the engine
     * defined by the end-user.
     */
    class ConfigManager {
        typedef std::unordered_map<int, NodeID> EntityMap;

        public:
            /**
             * @brief This method allows the user to load a Scene through its configuration file (.yaml).
             * @param _manager Manager of the engine
             * @param _scene Scene
             * @param _window Window
             * @param _configFilePath Path to the .yaml file that defines the Scene
             */
            void loadScene(Manager* _manager, Scene* _scene, Window* _window, const std::string& _configFilePath);

            /**
             * @brief This method allows to load a custom configuration for the engine.
             * @param _config New Configuration for the engine
             */
            void loadGDEConfig(GDEConfig* _config, FileManager& _manager);

        private:
            /**
             * @brief Loads a prefab in memory.
             * @param _scene Scene
             * @param _yaml Yaml file
             */
            void loadPrefab(Scene* _scene, Window* _window, YAML::Node& _yaml);

            /**
             * @brief Instantiates a previously loaded prefab.
             * @param _scene Scene
             * @param _yaml Yaml file
             */
            void instantiatePrefab(Scene* _scene, const YAML::Node& _yaml);

            /**
             * @brief Sets the parents for each entity if needed.
             * @param _map Entity map
             * @param _scene Scene
             * @param _yaml Yaml file
             */
            void parentingEntities(const EntityMap& _map, Scene* _scene, const YAML::Node& _yaml);

            void loadNodes(Scene* _scene, Window* _window, const nlohmann::json& _json);

            /**
             * @brief Loads all the assets defined in the .yaml for the Scene.
             * @param _scene Scene
             * @param _manager Engine Manager
             * @param _yaml Yaml file
             * @return EntityMap
             */
            void loadAssets(Scene* _scene, Window* _window, const nlohmann::json& _json);

            void loadTransformComponent(Scene* _scene, const NodeID& _nodeID, const nlohmann::json& _transformJson);

            void loadCameraComponent(const NodeID& _nodeID, Scene* _scene, Window* _window, const nlohmann::json& _cameraJson);

            void loadSpriteRendererComponent(const NodeID& _nodeID, Scene* _scene, const nlohmann::json& _spriteRendererJson);

            void loadTextRendererComponent(const NodeID& _nodeID, Scene* _scene, const nlohmann::json& _textRendererJson);

            void loadBodyComponent(const NodeID& _nodeID, Scene* _scene, const nlohmann::json& _bodyJson);

    };

}

#endif //ENGINE_CONFIG_LOADER_H
