// Created by borja on 9/3/22.


#ifndef ENGINE_SCENE_H
#define ENGINE_SCENE_H

#include <vector>
#include <string>
#include "core/Core.h"
#include "entt/entt.hpp"
#include "core/util/Delta.h"
#include "core/render/window/event/Event.h"

namespace engine {

    typedef entt::entity NodeID;

    class Node;
    class Scene {
        friend class Node;
        private:
            NodeID sceneRoot;
            std::string name;
            entt::registry registry;

            void printScene(const entt::entity&, std::ostream& _os, int& _indent);

        public:
            explicit Scene(const std::string& _sceneName);
            ~Scene() = default;

            void onEvent(Event& _event);
            void onUpdate(Delta _dt);
            void onFixedUpdate(Delta _dt);
            void onRender();

            Node* createNode(const std::string& _tag = "", Node* _parent = nullptr);
            Node* createSpriteNode(const std::string& _tag, Node* _parent = nullptr);

            void removeNode(const Node& _node);
            void removeNode(const std::string& _nodeTagName);

            Node* getNode(const std::string& _tagName);
            Node* getNode(const NodeID& _nodeID);

            void setParent(const Node& _node, Node& _parent);


            template<typename Component, typename... Args>
            Component* addComponent(const NodeID& _id, Args... _args);

            template<typename Component>
            void removeComponent(const NodeID& _id);

            template<typename Component>
            Component* getComponent(const NodeID& _id);


            std::string toString();
    };

    template<typename Component, typename... Args>
    Component* Scene::addComponent(const NodeID& _id, Args... _args) {
        return &registry.template emplace<Component>(_id, _args...);
    }

    template<typename Component>
    void Scene::removeComponent(const NodeID& _id) {
        registry.template remove<Component>(_id);
    }

    template<typename Component>
    Component* Scene::getComponent(const NodeID& _id) {
        return &registry.template get<Component>(_id);
    }
}

#endif //ENGINE_SCENE_H
