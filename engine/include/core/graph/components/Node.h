//
// Created by borja on 10/25/22.
//

#ifndef GDE_NODE_H
#define GDE_NODE_H

#include "core/graph/Graph.h"
#include "core/graph/components/Components.h"

FORWARD_DECLARE_CLASS(Manager)

namespace GDE {

    struct DisabledConfig {
        bool disabledForEvent = false;
        bool disabledForUpdate = false;
        bool disabledForFixedUpdate = false;
        bool disabledForRender = true;
        bool disabledForLateUpdate = false;
    };

    struct Node {
        FRIEND_CLASS(ImGuiScene, UISlider)

        private:
            Graph* graph = nullptr;
            Manager* manager = nullptr;
            NodeID ID = NODE_ID_NULL;
            Transform* transform = nullptr;

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
            bool hasComponent() const {
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

            void setEnabled(const DisabledConfig& _disabledConfig) {
                if(!getTransform()->parentTransform) return;
                if(!getTransform()->parentTransform->node->isEnabled()) return;

                enableOrDisableAllChildren(_disabledConfig, this);
            }

            [[nodiscard]] bool isEnabled() const {
                return  !hasComponent<DisabledForEvent>()       &&
                        !hasComponent<DisabledForUpdate>()      &&
                        !hasComponent<DisabledForFixedUpdate>() &&
                        !hasComponent<DisabledForRender>()      &&
                        !hasComponent<DisabledForLateUpdate>();
            }

            void overrideParentDisabledConfig(const DisabledConfig& _disabledConfig) {
                applyConfig(_disabledConfig, this);
            }

        private:
            void applyConfig(const DisabledConfig& _disabledConfig, Node* _node) {
                if(!_disabledConfig.disabledForEvent) {
                    if(_node->hasComponent<DisabledForEvent>()) _node->removeComponent<DisabledForEvent>();
                } else {
                    if(!_node->hasComponent<DisabledForEvent>()) _node->addComponent<DisabledForEvent>();
                }

                if(!_disabledConfig.disabledForUpdate) {
                    if(_node->hasComponent<DisabledForUpdate>()) _node->removeComponent<DisabledForUpdate>();
                } else {
                    if(!_node->hasComponent<DisabledForUpdate>()) _node->addComponent<DisabledForUpdate>();
                }

                if(!_disabledConfig.disabledForFixedUpdate) {
                    if(_node->hasComponent<DisabledForFixedUpdate>()) _node->removeComponent<DisabledForFixedUpdate>();
                } else {
                    if(!_node->hasComponent<DisabledForFixedUpdate>()) _node->addComponent<DisabledForFixedUpdate>();
                }

                if(!_disabledConfig.disabledForRender) {
                    if(_node->hasComponent<DisabledForRender>()) _node->removeComponent<DisabledForRender>();
                } else {
                    if(!_node->hasComponent<DisabledForRender>()) _node->addComponent<DisabledForRender>();
                }

                if(!_disabledConfig.disabledForLateUpdate) {
                    if(_node->hasComponent<DisabledForLateUpdate>()) _node->removeComponent<DisabledForLateUpdate>();
                } else {
                    if(!_node->hasComponent<DisabledForLateUpdate>()) _node->addComponent<DisabledForLateUpdate>();
                }
            }

            void enableOrDisableAllChildren(const DisabledConfig& _disabledConfig, Node* _node) {
                applyConfig(_disabledConfig, _node);
                for(auto* _child : _node->getTransform()->children) {
                    _child->node->enableOrDisableAllChildren(_disabledConfig, _child->node);
                }
            }
    };

}

#endif //GDE_NODE_H
