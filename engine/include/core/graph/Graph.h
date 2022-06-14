// Created by borja on 9/3/22.


#ifndef ENGINE_GRAPH_H
#define ENGINE_GRAPH_H

#include <entt/entt.hpp>
#include "core/util/Delta.h"
#include "core/render/window/event/Event.h"
#include "core/render/elements/Texture.h"

namespace GDE {

    /// This class is a main concept to understand about the engine. It manages how components update and it does it in a
    /// ECS way. That said, this engine doesn't force you to code in a ODD, you can perfectly work on a OPP way, although
    /// the engine provides you with the right tools to be able to work in a ODD way if wanted.
    ///
    /// The system works with Nodes, a Node is an entity that lives in the world and that is owned by a specific scene.
    /// This Node by itself isn't much, tho it has information about it's transform.
    ///
    /// When creating a new Node (via createNode(...)) it is appending a new entity to the world and it will give us a
    /// reference to the Node crated. This reference is no more than a uint value, but this graph is ready to make all
    /// kind of operations with this ID, so don't loose it!
    ///
    /// With the graph we control each Node and we can add, get, modify and remove components from the core of the engine
    /// or even custom components created by the end users.
    ///
    /// There is also a parenting system, so you can make a Node child of other Node. The scene will have a non-mutable
    /// root Node that will be the parent of all the other Nodes.
    ///
    /// RootNode
    ///     |__ MainCamera
    ///     |__ Player
    ///     |   |__ HealthBar
    ///     |   |__ Money
    ///     |__ Enemy

    typedef entt::entity NodeID;
    typedef entt::registry NodeContainer;
    #define NODE_ID_NULL entt::null

    class Engine; class Scene;
    class Graph {

        friend class Scene;

        private:
            NodeID sceneRoot;
            std::string name;
            NodeContainer registry;
            Scene* scene = nullptr;

        private:
            void printScene(const NodeID&, std::ostream& _os, int& _indent);
            void remove(const NodeID& _node, bool _delete);
            void onEvent(Event& _event);
            void onUpdate(Delta _dt);
            void onFixedUpdate(Delta _dt);
            void onRender();
            void onDebugRender();

        public:
            /// A callback if we want our graph to make any specific task during its internal event polling.
            UDelegate<void(Event&)> onEventDel;
            /// A callback if we want our graph to make any specific task during its internal update.
            UDelegate<void(Delta)> onUpdateDel;
            /// A callback if we want our graph to make any specific task during its internal fixed update.
            UDelegate<void(Delta)> onFixedUpdateDel;
            /// A callback if we want our graph to make any specific task during its internal render.
            UDelegate<void()> onRenderDel;

        public:
            Graph(Scene* _scene, const std::string& _sceneName);
            ~Graph() = default;

            /// This function creates and adds a new Node to the world. It is really recommended to add a tag value,
            /// as there is a way to retrieve a Node by it's tag name.
            /// @param _tag the name to reference if the ID is not provided.
            /// @param _parent the NodeID of the Node we want to be the parent of the creating Node.
            NodeID createNode(const std::string& _tag = "", const NodeID& _parent = NODE_ID_NULL);

            /// Removes a node and all of it's children.
            void removeNode(const NodeID& _node);
            /// Removes a node and all of it's children.
            void removeNode(const std::string& _nodeTagName);
            /// Makes the Node to loose it's parent and to be reassigned to the root Node.
            void orphan(const NodeID& _node);
            /// Makes the Node to loose it's parent and to be reassigned to the root Node.
            void orphan(const std::string& _nodeTagName);

            /// Returns the NddeID of a Node by it's tag name.
            NodeID getNode(const std::string& _tagName);

            /// Changes the parent of the Node to another one.
            void setParent(const NodeID& _node, const NodeID& _parent);

            /// Adds a new component to the Node. A Node cannot have duplicated components.
            /// @param _args all of the arguments needed by the component constructor.
            template<typename Component, typename... Args>
            Component* addComponent(Args... _args);

            /// Removes a component from the Node.
            template<typename Component>
            void removeComponent(const NodeID& _id);

            /// Returns a pointer to a component of Node. This pointer can be stored to always have track of the component
            /// and modify it at will.
            template<typename Component>
            Component* getComponent(const NodeID& _id);

            template<typename Component>
            bool hasComponent(const NodeID& _id);

            /// This function returns the graph tree as a string.
            std::string toString();
            /// Returns the ID of the root Node.
            NodeID getID();


            void setNodeStatic(NodeID _node, bool _static);
            bool isNodeStatic(NodeID _node);

            void setNodeActive(NodeID _node, bool _active);
            bool isNodeActive(NodeID _node);

            /// This is used to create the custom ECS work flow, more information on ECSManager.
            template<typename... Archetype>
            auto query();

            /// Returns the registry of EnTT that contains the Nodes. This is almost 100% sure NOT needed by end users.
            NodeContainer& getNodeContainer();
    };

    template <int I, class... Ts>
    decltype(auto) get(Ts&&... ts) {
        return std::get<I>(std::forward_as_tuple(ts...));
    }


    template<typename Component, typename... Args>
    Component* Graph::addComponent(Args... _args) {
        auto& first = get<0>(std::forward<Args>(_args)...);
        return &registry.template emplace<Component>(first, _args...);
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

    template<typename Component>
    bool Graph::hasComponent(const NodeID& _id) {
        return registry.template any_of<Component>(_id);
    }
}

#endif //ENGINE_GRAPH_H
