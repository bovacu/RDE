// Created by borja on 27/12/21.


#ifndef RDE_COMPONENTS_H
#define RDE_COMPONENTS_H


#include "core/graph/components/ComponentBase.h"
#include "core/Core.h"
#include "entt/entity/entity.hpp"

typedef entt::entity NodeID;

namespace RDE {
    class Manager;
    class Window;
    class Camera;
    class Canvas;
    class IViewPort;
    class Graph;
        
    struct Node;

    /**
     * @brief Component common to every entity and that identifies it whit a name. It is recommended 
     * every entity to have a different one.
     */
    struct Tag {
        public:
            /**
            * @brief Name of the entity.
            */
            std::string tag;

			RDE_FUNC Tag() {};
			RDE_FUNC Tag(const Tag& _tag) = default;
			RDE_FUNC Tag(const NodeID& _nodeId, const std::string& _tag) : tag(_tag) {  }

            explicit operator std::string& () { return tag; }
    };

    /**
     * @brief Component that tells the internal ECS system if an entity should or not be updated in the different systems.
     */
    struct Active {
		RDE_FUNC_EXPLICIT Active(Node* _node, Manager* _manager, Graph* _graph) {  }

        private:
            /**
             * @brief This element is needed, otherwise an empty struct won't compile (don't know exactly why).
             */
            short foo{};
    };


    struct DisabledForRender {
		RDE_FUNC_EXPLICIT DisabledForRender(Node* _node, Manager* _manager, Graph* _graph) {  }

        private:
            /**
             * @brief This element is needed, otherwise an empty struct won't compile (don't know exactly why).
             */
            short foo{};
    };

    struct DisabledForEvent {
		RDE_FUNC_EXPLICIT DisabledForEvent(Node* _node, Manager* _manager, Graph* _graph) {  }

        private:
            /**
             * @brief This element is needed, otherwise an empty struct won't compile (don't know exactly why).
             */
            short foo{};
    };

    struct DisabledForUpdate {
		RDE_FUNC_EXPLICIT DisabledForUpdate(Node* _node, Manager* _manager, Graph* _graph) {  }

        private:
            /**
             * @brief This element is needed, otherwise an empty struct won't compile (don't know exactly why).
             */
            short foo{};
    };

    struct DisabledForFixedUpdate {
		RDE_FUNC_EXPLICIT DisabledForFixedUpdate(Node* _node, Manager* _manager, Graph* _graph) {  }

        private:
            /**
             * @brief This element is needed, otherwise an empty struct won't compile (don't know exactly why).
             */
            short foo{};
    };

    struct DisabledForLateUpdate {
		RDE_FUNC_EXPLICIT DisabledForLateUpdate(Node* _node, Manager* _manager, Graph* _graph) {  }

        private:
            /**
             * @brief This element is needed, otherwise an empty struct won't compile (don't know exactly why).
             */
            short foo{};
    };

    /**
     * @brief Component used to block the event that the entity containing this component handled. If an entity
     * handles an event without this component, the event will propagate in deeper elements.
     * 
     */
    struct CanvasEventStopper : public ComponentBase {
		RDE_FUNC_EXPLICIT CanvasEventStopper(Node* _node, Manager* _manager, Graph* _graph) {  }

        RDE_FUNC void setEnabled(bool _enabled) override {
            enabled = _enabled;
        }

        RDE_FUNC_ND bool isEnabled() override {
            return enabled;
        }

        private:
            /**
             * @brief This element is needed, otherwise an empty struct won't compile (don't know exactly why).
             */
            bool enabled = true;
    };
}

#endif //RDE_COMPONENTS_H
