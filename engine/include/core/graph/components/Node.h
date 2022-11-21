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

            // TODO: add to each component of the engine which main functions they use.
            /**
             * @brief Enables or disables and specific main function of the node. The main functions are:
             *          - Event
             *          - Update
             *          - Fixed Update
             *          - Late Update
             *          - Render
             *
             *        Not all of the nodes may use internally all of the main functions. Usually if it is UI and interactable
             *        it will use both Event and Update and all of them use Render. Non UI elements can use any of them. Check
             *        the documentation for each specific one.
             *
             *        The uint8_t expected is a value of type EnabledStates. More than one can be passed at the same time
             *        via |, &, ^... operators.
             *
             *        Passing the states to the function will clean up the current flags, so if you want to keep the current
             *        enabled flags, pass it as getEnabledFlags() |/&/^ _enabledStates.
             *
             *        To disable, it is available INVERSE_ENABLED_STATE(x) which will return the selected flag with the bits
             *        inverted. For example, RENDER is 10000 (1 << 4), the inverse would be 01111.
             *
             *        So for example, if I want to disable the render flag: setEnabled(INVERSE_ENABLED_STATE(EnabledStates::RENDER));
             *        This will clear the current flags and set everything enabled but rendering.
             *
             *        If we want to keep the previous flags and also remove rendering: setEnabled(getEnabledFlags & INVERSE_ENABLED_STATE(EnabledStates::RENDER));
             *
             *        The enabled flags of the current node are passed to the children. If a specific flag of the parent is not desired
             *        to be inherited by the children, you can override it using 'overrideParentDisabledConfig'
             *
             * @param _enabledStates States to be enabled or disabled.
             */
            void setEnabled(uint8_t _enabledStates) {
                if(!getTransform()->parentTransform) return;
                if(!getTransform()->parentTransform->node->isEnabled()) return;

                enabledStates = _enabledStates;

                enableOrDisableAllChildren(_enabledStates, this);
            }

            /**
             * @brief Returns true if all of the flags are enabled.
             * @return bool
             */
            [[nodiscard]] bool isEnabled() const {
                return enabledStates == EnabledStates::DS_ALL;
            }

            /**
             * @brief Checks for a specific flag of EnabledStates. No more than one value should be passed at once!
             * @param _enabledState Flag to be checked.
             * @return bool
             */
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

            /**
             * @brief This is used to override an inherited enabled state of the parent.
             * @param _enabledStates One or more flags of type EnabledStates.
             */
            void overrideParentEnabledStates(uint8_t _enabledStates) {
                applyConfig(_enabledStates, this);
            }

            [[nodiscard]] uint8_t getEnabledFlags() const {
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
