// Created by borja on 9/3/22.


#ifndef ENGINE_GRAPH_H
#define ENGINE_GRAPH_H

#include <entt/entt.hpp>
#include "core/util/Delta.h"
#include "core/render/window/event/Event.h"
#include "core/render/elements/Texture.h"

namespace GDE {

    typedef entt::entity NodeID;
    #define NODE_ID_NULL entt::null

    class Graph {
        private:
            NodeID sceneRoot;
            std::string name;
            entt::registry registry;

        private:
            void printScene(const NodeID&, std::ostream& _os, int& _indent);
            void remove(const NodeID& _node, bool _delete);

        public:
            UDelegate<void(Event&)> onEventDel;
            UDelegate<void(Delta)> onUpdateDel;
            UDelegate<void(Delta)> onFixedUpdateDel;
            UDelegate<void()> onRenderDel;

        public:
            explicit Graph(const std::string& _sceneName);
            ~Graph() = default;

            void onEvent(Event& _event);
            void onUpdate(Delta _dt);
            void onFixedUpdate(Delta _dt);
            void onRender();

            NodeID createNode(const std::string& _tag = "", const NodeID& _parent = NODE_ID_NULL);
            NodeID createSpriteNode(Texture* _texture, const std::string& _tag = "", const NodeID& _parent = NODE_ID_NULL);

            void removeNode(const NodeID& _node);
            void removeNode(const std::string& _nodeTagName);
            void orphan(const NodeID& _node);
            void orphan(const std::string& _nodeTagName);

            NodeID getNode(const std::string& _tagName);

            void setParent(const NodeID& _node, const NodeID& _parent);

            template<typename Component, typename... Args>
            Component* addComponent(const NodeID& _id, Args... _args);

            template<typename Component>
            void removeComponent(const NodeID& _id);

            template<typename Component>
            Component* getComponent(const NodeID& _id);

            std::string toString();
            NodeID getID();

            void setNodeStatic(NodeID _node, bool _static);
            bool isNodeStatic(NodeID _node);

            void setNodeActive(NodeID _node, bool _active);
            bool isNodeActive(NodeID _node);

            template<typename... Archetype>
            auto query();
    };

    template<typename Component, typename... Args>
    Component* Graph::addComponent(const NodeID& _id, Args... _args) {
        return &registry.template emplace<Component>(_id, _args...);
    }

    template<typename Component>
    void Graph::removeComponent(const NodeID& _id) {
        registry.template remove<Component>(_id);
    }

    template<typename Component>
    Component* Graph::getComponent(const NodeID& _id) {
        return &registry.template get<Component>(_id);
    }

    template<typename... Archetype>
    auto Graph::query() {
        return registry.template view<Archetype...>().each();
    }
}

#endif //ENGINE_GRAPH_H
