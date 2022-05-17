//
// Created by borja on 15/05/22.
//

#ifndef ENGINE_CONFIG_LOADER_H
#define ENGINE_CONFIG_LOADER_H

#include <yaml-cpp/yaml.h>
#include "core/graph/Scene.h"

namespace GDE {

    class Window;
    struct GDEConfig;
    class ConfigManager {
        typedef std::unordered_map<int, NodeID> EntityMap;

        public:
            static void loadScene(Manager* _manager, Scene* _scene, Window* _window, const std::string& _configFilePath);
            static void loadGDEConfig(GDEConfig* _config);

        private:
            static EntityMap createEntities(Scene* _scene, const YAML::Node& _yaml);
            static void loadCameras(const EntityMap& _map, Scene* _scene, Window* _window, const YAML::Node& _yaml);
            static void loadSprites(Manager* _manager, const EntityMap& _map, Scene* _scene, const YAML::Node& _yaml);
            static void loadTextRenderers(Manager* _manager, const EntityMap& _map, Scene* _scene, const YAML::Node& _yaml);
            static void loadBodies(const EntityMap& _map, Scene* _scene, const YAML::Node& _yaml);
            static void loadAssets(Manager* _manager, const YAML::Node& _yaml);
            static void setBaseComponents(Scene* _scene, const NodeID& _nodeID, const YAML::Node& _yamlNode);
    };

}

#endif //ENGINE_CONFIG_LOADER_H
