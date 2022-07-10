//
// Created by borja on 15/05/22.
//

#ifndef ENGINE_CONFIG_LOADER_H
#define ENGINE_CONFIG_LOADER_H

#include "yaml-cpp/yaml.h"
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
             * @brief Creates all the entities defined in the .yaml of the Scene.
             * @param _scene Scene
             * @param _yaml Yaml file
             * @return EntityMap
             */
            EntityMap createEntities(Scene* _scene, const YAML::Node& _yaml);

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

            /**
             * @brief Loads all the cameras defined in the .yaml for the Scene.
             * @param _map Entity Map
             * @param _scene Scene
             * @param _window Window
             * @param _yaml Yaml file
             */
            void loadCameras(const EntityMap& _map, Scene* _scene, Window* _window, const YAML::Node& _yaml);

            /**
             * @brief Loads all the sprites defined in the .yaml for the Scene.
             * @param _manager Engine Manager
             * @param _map Entity Map
             * @param _scene Scene
             * @param _yaml Yaml file
             */
            void loadSprites(Manager* _manager, const EntityMap& _map, Scene* _scene, const YAML::Node& _yaml);

            /**
             * @brief Loads all the texts defined in the .yaml for the Scene.
             * @param _manager Engine Manager
             * @param _map Entity Map
             * @param _scene Scene
             * @param _yaml Yaml file
             */
            void loadTextRenderers(Manager* _manager, const EntityMap& _map, Scene* _scene, const YAML::Node& _yaml);

            /**
             * @brief Loads all the bodies defined in the .yaml for the Scene.
             * @param _map Entity Map
             * @param _scene Scene
             * @param _yaml Yaml file
             */
            void loadBodies(const EntityMap& _map, Scene* _scene, const YAML::Node& _yaml);

            /**
             * @brief Loads all the assets defined in the .yaml for the Scene.
             * @param _scene Scene
             * @param _manager Engine Manager
             * @param _yaml Yaml file
             * @return EntityMap
             */
            void loadAssets(Scene* _scene, Window* _window, const YAML::Node& _yaml);

            /**
             * @brief Sets the Tag, Transform and Active components.
             * @param _scene Scene
             * @param _nodeID Node to add the components
             * @param _yamlNode Yaml file
             */
            void setBaseComponents(Scene* _scene, const NodeID& _nodeID, const YAML::Node& _yamlNode);

            void loadCamera(const NodeID& _nodeID, Scene* _scene, Window* _window, const YAML::Node& _yaml);

            void loadSpriteRenderer(const NodeID& _nodeID, Scene* _scene, const YAML::Node& _yaml);

            void loadTextRenderer(const NodeID& _nodeID, Scene* _scene, const YAML::Node& _yaml);

            void loadBody(const NodeID& _nodeID, Scene* _scene, const YAML::Node& _yaml);

    };

}

#endif //ENGINE_CONFIG_LOADER_H
