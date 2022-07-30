// Created by borja on 9/3/22.


#ifndef ENGINE_GRAPH_H
#define ENGINE_GRAPH_H

#include "entt/single_include/entt/entt.hpp"
#include "core/util/Delta.h"
#include "core/systems/eventSystem/Event.h"
#include "core/render/elements/Texture.h"

namespace GDE {

    /** This class is a main concept to understand about the engine. It manages how components update and it does it in a
     * ECS way. That said, this engine doesn't force you to code in a ODD, you can perfectly work on a OPP way, although
     * the engine provides you with the right tools to be able to work in a ODD way if wanted.
     *
     * The system works with Nodes, a Node is an entity that lives in the world and that is owned by a specific scene.
     * This Node by itself isn't much, tho it has information about it's transform.
     *
     * When creating a new Node (via createNode(...)) it is appending a new entity to the world and it will give us a
     * reference to the Node crated. This reference is no more than a uint value, but this graph is ready to make all
     * kind of operations with this ID, so don't loose it!
     *
     * With the graph we control each Node and we can add, get, modify and remove components from the core of the engine
     * or even custom components created by the end users.
     *
     * There is also a parenting system, so you can make a Node child of other Node. The scene will have a non-mutable
     * root Node that will be the parent of all the other Nodes.
     *
     * RootNode
     *     |__ MainCamera
     *     |__ Player
     *     |   |__ HealthBar
     *     |   |__ Money
     *     |__ Enemy
     */

    typedef entt::entity NodeID;
    typedef entt::registry NodeContainer;
    #define NODE_ID_NULL entt::null

    class Engine; class Scene;
    class Graph {

        friend class Scene;

        private:
            /**
             * @brief Node that is the root of the scene
             */
            NodeID sceneRoot;

            /**
             * @brief Name of the root of the scene.
             */
            std::string name;

            /**
             * @brief EnTT entities container.
             */
            NodeContainer registry;

            /**
             * @brief Scene that this graph is attached to.
             */
            Scene* scene = nullptr;

        private:
            /**
             * @brief Prints the hierarchy tree on the console.
             * @param _os
             * @param _indent
             */
            void printScene(const NodeID&, std::ostream& _os, int& _indent);

            /**
             * @brief Removes a node all of its components and also can destroy it from the scene.
             * @param _node Node to remove components
             * @param _delete If also has to be destroyed from the scene
             */
            void remove(const NodeID& _node, bool _delete);

            /**
             * @brief Manages the events that the graph must interact with.
             * @param _event Event to handle.
             */
            void onEvent(Event& _event);

            /**
             * @brief Updates the entities in the graph.
             * @param _dt Delta time
             */
            void onUpdate(Delta _dt);

            /**
             * @brief Updates the entities's physics in the graph.
             * @param _dt Delta time
             */
            void onFixedUpdate(Delta _dt);

            /**
             * @brief Renders the entities in the graph
             */
            void onRender();

            /**
             * @brief Renders debug information of the entities in the graph.
             */
            void onDebugRender();

        public:
            /**
             * @brief A callback if we want our graph to make any specific task during its internal event polling.
             */
            UDelegate<void(Event&)> onEventDel;

            /**
             * @brief A callback if we want our graph to make any specific task during its internal update.
             */
            UDelegate<void(Delta)> onUpdateDel;

            /**
             * @brief A callback if we want our graph to make any specific task during its internal fixed update.
             */
            UDelegate<void(Delta)> onFixedUpdateDel;

            /**
             * @brief A callback if we want our graph to make any specific task during its internal render.
             */
            UDelegate<void()> onRenderDel;

        public:
            Graph(Scene* _scene, const std::string& _sceneName);
            ~Graph() {};

            /** This function creates and adds a new Node to the world. It is really recommended to add a tag value,
             * as there is a way to retrieve a Node by it's tag name.
             * @param _tag the name to reference if the ID is not provided.
             * @param _parent the NodeID of the Node we want to be the parent of the creating Node.
             */
            NodeID createNode(const std::string& _tag = "", const NodeID& _parent = NODE_ID_NULL);

            /**
             * @brief Creates an independent copy of a loaded prefab.
             * @param _prefab Prefab NodeID
             * @return NodeID
             */
            NodeID instantiatePrefab(const NodeID& _prefab, const Vec2F& _position, const NodeID& _parent = NODE_ID_NULL);

            /**
             * @brief Removes a node and all of it's children.
             * @param _node Id of the node to remove
             */
            void removeNode(const NodeID& _node);

            /**
             * @brief Removes a node and all of it's children.
             * @param _nodeTagName Name of the node to remove
             */
            void removeNode(const std::string& _nodeTagName);

            /**
             * @brief Makes the Node to loose it's parent and to be reassigned to the root Node.
             * @param _node Id of the node to orphan
             */
            void orphan(const NodeID& _node);

            /**
             * @brief Makes the Node to loose it's parent and to be reassigned to the root Node.
             * @param _nodeTagName Name of the node to orphan
             */
            void orphan(const std::string& _nodeTagName);

            /**
             * @brief Returns the NodeID of a Node by it's tag name.
             * @param _tagName Name of the node
             * @return NodeID
             */
            NodeID getNode(const std::string& _tagName);

            /**
             * @brief Changes the parent of the Node to another one.
             * @param _node Id of the child
             * @param _parent Id of the parent
             */
            void setParent(const NodeID& _node, const NodeID& _parent);

            /**
             * @brief Adds a new component to the Node. A Node cannot have duplicated components.
             * @tparam Component A component type
             * @tparam Args Arguments that the component needs in the constructor
             * @param _args all of the arguments needed by the component constructor
             */
            template<typename Component, typename... Args>
            Component* addComponent(Args... _args);

            /**
             * @brief Removes a component from the Node.
             * @tparam Component A component type
             * @param _id Id of the node to remove the component
             */
            template<typename Component>
            void removeComponent(const NodeID& _id);

            /**
             * @brief Returns a pointer to a component of Node. This pointer can be stored to always have track of the component
             * and modify it at will.
             * @tparam Component
             * @param _id Id of the node to get a component.
             * @return Component*
             */
            template<typename Component>
            Component* getComponent(const NodeID& _id);

            /**
             * @brief Returns true of the Node has the component
             * @tparam Component A component type
             * @param _id Id of the node
             * @return bool
             */
            template<typename Component>
            bool hasComponent(const NodeID& _id);

            /**
             * @brief This function returns the graph tree as a string.
             * @return std::string
             */
            std::string toString();
            /// Returns the ID of the root Node.

            /**
             * @brief Returns the Id of the root.
             * @return NodeID
             */
            NodeID getID();

            /**
             * @brief Sets a node to static or unsets it.
             * @param _node Node to made or unmade static.
             * @param _static If the node is static or not.
             */
            void setNodeStatic(NodeID _node, bool _static);

            /**
             * @brief Returns if the node is static.
             * @param _node Node to check if is static
             * @return bool
             */
            bool isNodeStatic(NodeID _node);

            /**
             * @brief Enables or disables a node.
             * @param _node Node to be enabled or disabled.
             * @param _active If is enabled or disabled.
             */
            void setNodeActive(NodeID _node, bool _active);

            /**
             * @brief Returns if the node is active or not.
             * @param _node Node to check if is active.
             * @return bool
             */
            bool isNodeActive(NodeID _node);

            /**
             * @brief This is used to create the custom ECS work flow, more information on ECSManager.
             * @tparam Archetype The Components that the query must look for
             * @return A entt query
             */
            template<typename... Archetype>
            auto query();

            /**
             * @brief Returns the registry of EnTT that contains the Nodes. This is almost 100% sure NOT needed by end users.
             * @return NodeContainer
             */
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
