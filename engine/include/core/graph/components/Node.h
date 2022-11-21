//
// Created by borja on 10/25/22.
//

#ifndef GDE_NODE_H
#define GDE_NODE_H

#include "core/graph/Graph.h"
#include "core/graph/components/Components.h"

FORWARD_DECLARE_CLASS(Manager)

namespace GDE {

    enum EnabledStates {
        DS_NONE         =      0,
        DS_EVENT        = 1 << 0,
        DS_UPDATE       = 1 << 1,
        DS_FIXED_UPDATE = 1 << 2,
        DS_LATE_UPDATE  = 1 << 3,
        DS_RENDER       = 1 << 4,
        DS_ALL          = DS_EVENT | DS_UPDATE | DS_FIXED_UPDATE | DS_LATE_UPDATE | DS_RENDER
    };

#define INVERSE_ENABLED_STATE(x) (GDE::EnabledStates)(GDE::EnabledStates::DS_ALL ^ (x))

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

            void setEnabled(uint8_t _enabledStates) {
                if(!getTransform()->parentTransform) return;
                if(!getTransform()->parentTransform->node->isEnabled()) return;

                enabledStates = _enabledStates;

                enableOrDisableAllChildren(_enabledStates, this);
            }

            [[nodiscard]] bool isEnabled() const {
                return enabledStates == EnabledStates::DS_ALL;
            }

            [[nodiscard]] bool isEnabledStateOn(EnabledStates _enabledState) const {
                if(_enabledState == EnabledStates::DS_EVENT && (enabledStates & EnabledStates::DS_EVENT) == EnabledStates::DS_EVENT) {
                    return true;
                }

                if(_enabledState == EnabledStates::DS_UPDATE && (enabledStates & EnabledStates::DS_UPDATE) == EnabledStates::DS_UPDATE) {
                    return true;
                }

                if(_enabledState == EnabledStates::DS_FIXED_UPDATE && (enabledStates & EnabledStates::DS_FIXED_UPDATE) == EnabledStates::DS_FIXED_UPDATE) {
                    return true;
                }

                if(_enabledState == EnabledStates::DS_LATE_UPDATE && (enabledStates & EnabledStates::DS_LATE_UPDATE) == EnabledStates::DS_LATE_UPDATE) {
                    return true;
                }

                if(_enabledState == EnabledStates::DS_RENDER && (enabledStates & EnabledStates::DS_RENDER) == EnabledStates::DS_RENDER) {
                    return true;
                }

                return false;
            }

            void overrideParentDisabledConfig(EnabledStates _disabledStates) {
                enabledStates = _disabledStates;
                applyConfig(_disabledStates, this);
            }

            [[nodiscard]] uint8_t getDisabledFlags() const {
                return enabledStates;
            }

        private:
            void applyConfig(uint8_t _enabledStates, Node* _node) {
                enabledStates = _enabledStates;

                if((_enabledStates & EnabledStates::DS_EVENT) == EnabledStates::DS_EVENT) {
                    if(_node->hasComponent<DisabledForEvent>()) {
                        _node->removeComponent<DisabledForEvent>();
                    }
                } else {
                    if(!_node->hasComponent<DisabledForEvent>()) {
                        _node->addComponent<DisabledForEvent>();
                    }
                }

                if((_enabledStates & EnabledStates::DS_UPDATE) == EnabledStates::DS_UPDATE) {
                    if(_node->hasComponent<DisabledForUpdate>()) {
                        _node->removeComponent<DisabledForUpdate>();
                    }
                } else {
                    if(!_node->hasComponent<DisabledForUpdate>()) {
                        _node->addComponent<DisabledForUpdate>();
                    }
                }

                if((_enabledStates & EnabledStates::DS_FIXED_UPDATE) == EnabledStates::DS_FIXED_UPDATE) {
                    if(_node->hasComponent<DisabledForFixedUpdate>()) {
                        _node->removeComponent<DisabledForFixedUpdate>();
                    }
                } else {
                    if(!_node->hasComponent<DisabledForFixedUpdate>()) {
                        _node->addComponent<DisabledForFixedUpdate>();
                    }
                }

                if((_enabledStates & EnabledStates::DS_LATE_UPDATE) == EnabledStates::DS_LATE_UPDATE) {
                    if(_node->hasComponent<DisabledForLateUpdate>()) {
                        _node->removeComponent<DisabledForLateUpdate>();
                    }
                } else {
                    if(!_node->hasComponent<DisabledForLateUpdate>()) {
                        _node->addComponent<DisabledForLateUpdate>();
                    }
                }

                if((_enabledStates & EnabledStates::DS_RENDER) == EnabledStates::DS_RENDER) {
                    if(_node->hasComponent<DisabledForRender>()) {
                        _node->removeComponent<DisabledForRender>();
                    }
                } else {
                    if(!_node->hasComponent<DisabledForRender>()) {
                        _node->addComponent<DisabledForRender>();
                    }
                }
            }

            void enableOrDisableAllChildren(uint8_t _disabledStates, Node* _node) {
                applyConfig(_disabledStates, _node);
                for(auto* _child : _node->getTransform()->children) {
                    _child->node->enableOrDisableAllChildren(_disabledStates, _child->node);
                }
            }
    };

}

#endif //GDE_NODE_H
