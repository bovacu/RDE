//
// Created by borja on 15/05/22.
//

#ifndef RDE_CONFIG_LOADER_H
#define RDE_CONFIG_LOADER_H

#include "nlohmann/json.hpp"

namespace RDE {

    class Window;
    struct RDEConfig;
	class Manager;
	class Scene;
	class Node;
	class FileManager;

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
             * @param _rdeConfig Configuration of RDE
             * @param _manager Manager
             */
            void loadResources(RDEConfig* _rdeConfig, Manager* _manager);

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
            void loadRDEConfig(RDEConfig* _config, FileManager* _manager);

        private:
            /**
             * @brief Instantiates a previously loaded prefab.
             * @param _scene Scene
             * @param _json JSON file
             */
            void instantiatePrefab(Scene* _scene, const nlohmann::json& _json);

            /**
             * @brief Creates the Nodes using the json of the defined Scene.
             * @param _scene The scene being loaded
             * @param _sceneJson The json of the Scene containing all of the information
             * @return A map containing the key of the Node (tag) and the data as a value.
            */
            std::unordered_map<std::string, Node_JsonPair> createNodes(Scene* _scene, const nlohmann::json& _sceneJson);

            /**
             * @brief Loads all of the components that each Node has defined on the json file of the Scene.
             * @param _scene The scene being loaded
             * @param _sceneJson The json of the Scene containing all of the information
             * @param _nodes Map containing the key of the Node (tag) and the data as a value
            */
            void loadNodes(Scene* _scene, Window* _window, const nlohmann::json& _sceneJson, const std::unordered_map<std::string, Node_JsonPair>& _nodes);

            /**
             * @brief Loads all the assets defined in the .json for the Scene.
             * @param _manager Engine Manager
             * @param _json JSON file
             */
            void loadAssets(Manager* _manager, const nlohmann::json& _json);

            /**
             * @brief Loads a component of type Transform into the Node.
             * @param _scene The Scene being loaded
             * @param _node The node getting the component
             * @param _json The json containing the data to fill the Node's component
             */
            void loadTransformComponent(Scene* _scene, Node* _node, const nlohmann::json& _transformJson);

            /**
             * @brief Loads a component of type Camera into the Node.
             * @param _scene The Scene being loaded
             * @param _node The node getting the component
             * @param _json The json containing the data to fill the Node's component
             */
            void loadCameraComponent(Node* _nodeD, Scene* _scene, Window* _window, const nlohmann::json& _cameraJson);

            /**
             * @brief Loads a component of type SpriteRenderer into the Node.
             * @param _scene The Scene being loaded
             * @param _node The node getting the component
             * @param _json The json containing the data to fill the Node's component
             */
            void loadSpriteRendererComponent(Node* _node, Scene* _scene, const nlohmann::json& _spriteRendererJson);

            /**
             * @brief Loads a component of type TextRenderer into the Node.
             * @param _scene The Scene being loaded
             * @param _node The node getting the component
             * @param _json The json containing the data to fill the Node's component
             */
            void loadTextRendererComponent(Node* _node, Scene* _scene, const nlohmann::json& _textRendererJson);

            /**
             * @brief Loads a component of type PhysicsBody into the Node.
             * @param _scene The Scene being loaded
             * @param _node The node getting the component
             * @param _json The json containing the data to fill the Node's component
             */
            void loadPhysicsBodyComponent(Node* _node, Scene* _scene, const nlohmann::json& _bodyJson);

            /**
             * @brief Unloads all the assets of the Scene from memory.
             * @param _scene The Scene being unloaded
             * @param _json The json containing all the Scene assets
            */
            void unloadAssets(Scene* _scene, const nlohmann::json& _sceneJson);
    };

}

#endif //RDE_CONFIG_LOADER_H
