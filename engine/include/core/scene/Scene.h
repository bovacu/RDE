// Created by borja on 9/3/22.


#ifndef ENGINE_SCENE_H
#define ENGINE_SCENE_H

#include <vector>
#include <string>
#include "core/Core.h"
#include "entt/entity/registry.hpp"

namespace engine {

    class Node;
    class Scene {
        friend class Node;
        private:
            Node* sceneRoot;
            std::string name;
            entt::registry registry;
            std::vector<Node*> nodeTracker;

            void printScene(const entt::entity&, std::ostream& _os, int& _indent);

        public:
            explicit Scene(const std::string& _sceneName);
            ~Scene();

            Node& createNode(const std::string& _tag = "", Node* _parent = nullptr);
            void removeNode(const Node& _node);

            Node& createSpriteNode(const std::string& _tag, Node* _parent = nullptr);

            void setParent(const Node& _node, Node& _parent);

            void removeNode(const std::string& _nodeTagName);

            std::string toString();
    };

}

#endif //ENGINE_SCENE_H
