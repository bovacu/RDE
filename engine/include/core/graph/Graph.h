// Created by borja on 9/3/22.


#ifndef RDE_GRAPH_H
#define RDE_GRAPH_H

#include "entt/entt.hpp"
#include "core/util/Delta.h"
#include "core/util/Vec.h"
#include "core/systems/eventSystem/Event.h"
#include "core/graph/components/Components.h"
#include <tuple>

namespace RDE {

    /** This class is a main concept to understand about the engine. It manages how components update and it does it in a
     * ECS way. That said, this engine doesn't force you to code in a ODD, you can perfectly work on a OPP way, although
     * the engine provides you with the right tools to be able to work in a ODD way if wanted.
     *
     * The system works with Nodes, a Node is an entity that lives in the world and that is owned by a specific scene.
     * This Node by itself isn't much, tho it has information about it's transform.
     *
     * When creating a new Node (via createNode(...)) it is appending a new entity to the world and it will give us a
     * reference to the Node crated.
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

    class Engine; 
    class Scene; 
    class Manager;
	class Transform;

    struct Node; 
    struct RenderizableInnerData;

    struct RenderingTreeData {
        std::vector<std::tuple<RenderizableInnerData*, Transform*, void*>> sprites;
        std::vector<std::tuple<RenderizableInnerData*, Transform*, void*>> dynamicSprites;
        std::vector<std::tuple<RenderizableInnerData*, Transform*, void*>> particleSystmes;
        std::vector<std::tuple<RenderizableInnerData*, Transform*, void*>> texts;

        std::vector<std::tuple<RenderizableInnerData*, Transform*, void*>> uiImages;
        std::vector<std::tuple<RenderizableInnerData*, Transform*, void*>> uiTexts;

        std::vector<Transform*> dirtyTransforms;
        bool isRenderizableTreeDirty = false;
        bool isUI = false;
    };

    class Graph {
        friend class Transform;
        friend class Scene;
        friend class Canvas;
        friend class Node;
        friend class ImGuiScene;
        friend class ConfigManager;
        friend class RenderizableInnerData;

        private:
            /**
             * @brief Node that is the root of the scene
             */
            Node* sceneRoot;

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

            UniqueDelegate<void(void*)> onDataChanged;

            RenderingTreeData renderingTreeData;

        private:
            /**
             * @brief Prints the hierarchy tree on the console.
             * @param _os
             * @param _indent
             */
            void printScene(Node* _node, std::ostream& _os, int& _indent);

            /**
             * @brief Removes a node all of its components and also can destroy it from the scene.
             * @param _node Node to remove components
             * @param _delete If also has to be destroyed from the scene
             */
            void removeNodeInner(Node* _node, bool _delete = true);

        public:
            /**
             * @brief A callback if we want our graph to make any specific task during its internal event polling.
             */
            UniqueDelegate<void(NodeContainer&, Event&)> onEventDel;

        public:
            Graph(Scene* _scene, const std::string& _sceneName, bool _isUI = false);
            ~Graph() {};

            /** This function creates and adds a new Node to the world. It is really recommended to add a tag value,
             * as there is a way to retrieve a Node by it's tag name.
             * @param _tag the name to reference if the ID is not provided.
             * @param _parent the NodeID of the Node we want to be the parent of the creating Node.
             */
            Node* createNode(const std::string& _tag = "", Node* _parent = nullptr);

            /**
             * @brief Creates an independent copy of a loaded prefab.
             * @param _prefab Prefab NodeID
             * @return NodeID
             */
            Node* instantiatePrefab(Node* _prefab, const Vec2F& _position, Node* _parent = nullptr);

            /**
             * @brief Removes a node and all of it's children.
             * @param _node Id of the node to remove
             */
            void removeNode(Node* _node);

            /**
             * @brief Removes a node and all of it's children.
             * @param _nodeTagName Name of the node to remove
             */
            void removeNode(const std::string& _nodeTagName);

            /**
             * @brief Makes the Node to loose it's parent and to be reassigned to the root Node.
             * @param _node Id of the node to orphan
             */
            void orphan(Node* _node);

            /**
             * @brief Makes the Node to loose it's parent and to be reassigned to the root Node.
             * @param _nodeTagName Name of the node to orphan
             */
            void orphan(const std::string& _nodeTagName);

            /**
             * @brief Returns the Node* by it's tag name.
             * @param _tagName Name of the node
             * @return Node*
             */
            Node* getNode(const std::string& _tagName);

            /**
             * @brief Returns the Node* by it's NodeID.
             * @param _nodeID Name of the node
             * @return Node*
             */
            Node* getNode(const NodeID& _nodeID);

            /**
             * @brief Changes the parent of the Node to another one.
             * @param _node Id of the child
             * @param _parent Id of the parent
             */
            void setParent(Node* _node, Node* _parent);

            /**
             * @brief This function returns the graph tree as a string.
             * @return std::string
             */
            std::string toString();

            /**
             * @brief Returns the Id of the root.
             * @return NodeID
             */
            Node* getRoot();

            /**
             * @brief Enables or disables a node.
             * @param _node Node to be enabled or disabled.
             * @param _active If is enabled or disabled.
             */
            void setNodeActive(Node* _node, bool _active);

            /**
             * @brief Returns if the node is active or not.
             * @param _node Node to check if is active.
             * @return bool
             */
            bool isNodeActive(Node* _node);

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

    template<typename... Archetype>
    auto Graph::query() {
        #if IS_WINDOWS()
        return registry.view<Archetype...>().each();
        #else
        return registry.template view<Archetype...>().each();
        #endif
    }
}

#endif //RDE_GRAPH_H
