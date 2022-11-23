//
// Created by borja on 10/25/22.
//

#ifndef RDE_NODE_H
#define RDE_NODE_H

#include "core/graph/Graph.h"
#include "core/graph/components/Components.h"

FORWARD_DECLARE_CLASS(Manager)

namespace RDE {

    enum EnabledStates {
        DS_NONE         =      0,
        DS_EVENT        = 1 << 0,
        DS_UPDATE       = 1 << 1,
        DS_FIXED_UPDATE = 1 << 2,
        DS_LATE_UPDATE  = 1 << 3,
        DS_RENDER       = 1 << 4,
        DS_ALL          = DS_EVENT | DS_UPDATE | DS_FIXED_UPDATE | DS_LATE_UPDATE | DS_RENDER
    };

#define INVERSE_ENABLED_STATE(x) (RDE::EnabledStates)(RDE::EnabledStates::DS_ALL ^ (x))

    struct Node {
        FRIEND_CLASS(ImGuiScene, UISlider)

        private:
            Graph* graph = nullptr;
            Manager* manager = nullptr;
            NodeID ID = NODE_ID_NULL;
            Transform* transform = nullptr;
            uint8_t enabledStates = EnabledStates::DS_ALL;

        public:
            Node(const NodeID& _nodeID, Graph* _graph, Manager* _manager, Transform* _transform) : ID(_nodeID), graph(_graph), transform(_transform), manager(_manager) {  }

            template<typename Component, typename... Args>
            Component* addComponent(Args... _args) {
                ENGINE_ASSERT(!hasComponent<Component>(), "A Node cannot have 2 components of the same type!!")
                return graph->template addComponent<Component>(this, manager, graph, _args...);
            }

            template<typename Component>
            Component* getComponent() {
                return graph->template getComponent<Component>(ID);
            }

            template<typename Component>
            void removeComponent() {
                graph->template removeComponent<Component>(ID);
            }

            template<typename Component>
            [[nodiscard]] bool hasComponent() const {
                return graph->template hasComponent<Component>(ID);
            }

            Transform* getTransform() {
                return transform;
            }

            NodeID getID() {
                return ID;
            }

            void setParent(Node* _parent) {
                graph->setParent(this, _parent);
            }

            bool isActive() {
                return graph->isNodeActive(this);
            }

            void setActive(bool _active) {
                return graph->setNodeActive(this, _active);
            }
    };

}

#endif //RDE_NODE_H
