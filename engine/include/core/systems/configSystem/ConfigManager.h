//
// Created by borja on 15/05/22.
//

#ifndef GDE_CONFIG_LOADER_H
#define GDE_CONFIG_LOADER_H

#include "nlohmann/json.hpp"
#include "core/graph/Scene.h"
#include "core/systems/fileSystem/FileManager.h"

namespace GDE {

    FORWARD_DECLARE_CLASS(Window)
    FORWARD_DECLARE_STRUCT(GDEConfig)

    struct Node_JsonPair {
        Node* node;
        nlohmann::json json;
    };

    /**
     * @brief This class is in charge of generating the Scenes and set up the initial configurations of the engine
     * defined by the end-user.
     */
    class ConfigManager {
        public:
            /**
             * @brief Loads assets that need to be loaded between different scenes  (Must be a .json).
             * @param _gdeConfig Configuration of GDE
             * @param _manager Manager
             */
            void loadResources(GDEConfig* _gdeConfig, Manager* _manager);

            /**
             * @brief This method allows the user to load a Scene through its configuration file (Must be a .json).
             * @param _manager Manager of the engine
             * @param _scene Scene
             * @param _window Window
             * @param _configFilePath Path to the .yaml file that defines the Scene
             */
            void loadScene(Manager* _manager, Scene* _scene, Window* _window, const std::string& _configFilePath);

            /**
             * @brief This method allows the user to unload a Scene through its configuration file (Must be a .json).
             * @param _manager Manager of the engine
             * @param _scene Scene
             * @param _window Window
             * @param _configFilePath Path to the .yaml file that defines the Scene
             */
            void unloadScene(Manager* _manager, Scene* _scene, const std::string& _configFilePath);

            /**
             * @brief This method allows to load a custom configuration for the engine.
             * @param _config New Configuration for the engine
             */
            void loadGDEConfig(GDEConfig* _config, FileManager& _manager);

        private:
            /**
             * @brief Instantiates a previously loaded prefab.
             * @param _scene Scene
             * @param _json JSON file
             */
            void instantiatePrefab(Scene* _scene, const nlohmann::json& _json);

            std::unordered_map<std::string, Node_JsonPair> createNodes(Scene* _scene, const nlohmann::json& _sceneJson);

            void loadNodes(Scene* _scene, Window* _window, const nlohmann::json& _sceneJson, const std::unordered_map<std::string, Node_JsonPair>& _nodes);

            /**
             * @brief Loads all the assets defined in the .json for the Scene.
             * @param _scene Scene
             * @param _manager Engine Manager
             * @param _json JSON file
             * @return EntityMap
             */
            void loadAssets(Manager* _manager, const nlohmann::json& _json);

            void loadTransformComponent(Scene* _scene, Node* _node, const nlohmann::json& _transformJson);

            void loadCameraComponent(Node* _nodeD, Scene* _scene, Window* _window, const nlohmann::json& _cameraJson);

            void loadSpriteRendererComponent(Node* _node, Scene* _scene, const nlohmann::json& _spriteRendererJson);

            void loadTextRendererComponent(Node* _node, Scene* _scene, const nlohmann::json& _textRendererJson);

            void loadBodyComponent(Node* _node, Scene* _scene, const nlohmann::json& _bodyJson);

            void unloadAssets(Scene* _scene, const nlohmann::json& _sceneJson);
    };

}

#endif //GDE_CONFIG_LOADER_H
