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
        bool disabledForRender = false;
        bool disabledForLateUpdate = false;

        static DisabledConfig DisabledConfigEvent;
        static DisabledConfig DisabledConfigUpdate;
        static DisabledConfig DisabledConfigFixedUpdate;
        static DisabledConfig DisabledConfigRender;
        static DisabledConfig DisabledConfigLateUpdate;

        static DisabledConfig EnabledConfigEvent;
        static DisabledConfig EnabledConfigUpdate;
        static DisabledConfig EnabledConfigFixedUpdate;
        static DisabledConfig EnabledConfigRender;
        static DisabledConfig EnabledConfigLateUpdate;

        DisabledConfig operator ~() {
            return {
                .disabledForEvent       = !disabledForEvent,
                .disabledForUpdate      = !disabledForUpdate,
                .disabledForFixedUpdate = !disabledForFixedUpdate,
                .disabledForRender      = !disabledForRender,
                .disabledForLateUpdate  = !disabledForRender,
            };
        }
    };

    inline DisabledConfig operator |(const DisabledConfig& _config0, const DisabledConfig& _config1) {
        return {
                .disabledForEvent       = (_config0.disabledForEvent        || _config1.disabledForEvent),
                .disabledForUpdate      = (_config0.disabledForUpdate       || _config1.disabledForUpdate),
                .disabledForFixedUpdate = (_config0.disabledForFixedUpdate  || _config1.disabledForFixedUpdate),
                .disabledForRender      = (_config0.disabledForRender       || _config1.disabledForRender),
                .disabledForLateUpdate  = (_config0.disabledForLateUpdate   || _config1.disabledForLateUpdate),
        };
    }

    inline DisabledConfig operator &(const DisabledConfig& _config0, const DisabledConfig& _config1) {
        return {
                .disabledForEvent       = (_config0.disabledForEvent        && _config1.disabledForEvent),
                .disabledForUpdate      = (_config0.disabledForUpdate       && _config1.disabledForUpdate),
                .disabledForFixedUpdate = (_config0.disabledForFixedUpdate  && _config1.disabledForFixedUpdate),
                .disabledForRender      = (_config0.disabledForRender       && _config1.disabledForRender),
                .disabledForLateUpdate  = (_config0.disabledForLateUpdate   && _config1.disabledForLateUpdate),
        };
    }

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

            /**
             * @brief Enables or disables specific functionalities of a Node. If used on a Node with children, the configuration
             *        will be inherited to all of them, but can be changed through overrideParentDisabledConfig.
             *
             *        There are predefined configs inside DisabledConfig. You can join many of them by using |, &, ~ operators as needed.
             *
             *        Example:
             *              node->setDisabled(DisabledConfig::DisabledConfigRender | DisabledConfig::DisabledConfigUpdate);
             *
             * @param _disabledConfig
             */
            void setDisabled(const DisabledConfig& _disabledConfig = DisabledConfig::DisabledConfigRender) {
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

            [[nodiscard]] bool isEnabled(const DisabledConfig& _disabledConfig) const {
                if(!_disabledConfig.disabledForEvent) {
                    if(hasComponent<DisabledForEvent>()) return false;
                }

                if(!_disabledConfig.disabledForUpdate) {
                    if(hasComponent<DisabledForUpdate>()) return false;
                }

                if(!_disabledConfig.disabledForFixedUpdate) {
                    if(hasComponent<DisabledForFixedUpdate>()) return false;
                }

                if(!_disabledConfig.disabledForRender) {
                    if(hasComponent<DisabledForRender>()) return false;
                }

                if(!_disabledConfig.disabledForLateUpdate) {
                    if(hasComponent<DisabledForLateUpdate>()) return false;
                }

                return true;
            }

            void overrideParentDisabledConfig(const DisabledConfig& _disabledConfig) {
                applyConfig(_disabledConfig, this);
            }

            [[nodiscard]] DisabledConfig getDisabledConfig() const {
                return {
                    .disabledForEvent       = hasComponent<DisabledForEvent>(),
                    .disabledForUpdate      = hasComponent<DisabledForUpdate>(),
                    .disabledForFixedUpdate = hasComponent<DisabledForFixedUpdate>(),
                    .disabledForRender      = hasComponent<DisabledForRender>(),
                    .disabledForLateUpdate  = hasComponent<DisabledForLateUpdate>(),
                };
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
