// Created by borja on 9/3/22.


#ifndef ENGINE_SCENE_H
#define ENGINE_SCENE_H

#include <vector>
#include <string>
#include "core/Core.h"
#include "entt/entt.hpp"
#include "core/util/Delta.h"
#include "core/render/window/event/Event.h"
#include "core/render/elements/Texture.h"

namespace engine {

    typedef entt::entity NodeID;

    #define NODE_ID_NULL entt::null

    class Scene {
        friend class Node;
        private:
            NodeID sceneRoot;
            std::string name;
            entt::registry registry;

        private:
            void printScene(const NodeID&, std::ostream& _os, int& _indent);
            void insert(const NodeID& _node, const NodeID& _parent);
            void remove(const NodeID& _node, bool _delete);
            void sortHierarchyInMemory();

        public:
            explicit Scene(const std::string& _sceneName);
            ~Scene() = default;

            void onEvent(Event& _event);
            void onUpdate(Delta _dt);
            void onFixedUpdate(Delta _dt);
            void onRender();

            NodeID createNode(const std::string& _tag = "", const NodeID& _parent = NODE_ID_NULL);
            NodeID createSpriteNode(Texture* _texture, const std::string& _tag = "", const NodeID& _parent = NODE_ID_NULL);

            void removeNode(const NodeID& _node);
            void removeNode(const std::string& _nodeTagName);

            NodeID getNode(const std::string& _tagName);

            bool nodeIsLeaf(const NodeID& _nodeID);

            void setParent(const NodeID& _node, const NodeID& _parent);

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
