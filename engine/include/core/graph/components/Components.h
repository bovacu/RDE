// Created by borja on 27/12/21.


#ifndef RDE_COMPONENTS_H
#define RDE_COMPONENTS_H


#include "ComponentBase.h"
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

            Tag() {};
            Tag(const Tag& _tag) = default;
            Tag(const NodeID& _nodeId, const std::string& _tag) : tag(_tag) {  }

            explicit operator std::string& () { return tag; }
    };

    /**
     * @brief Component that tells the internal ECS system if an entity should or not be updated in the different systems.
     */
    struct Active {
        explicit Active(Node* _node, Manager* _manager, Graph* _graph) {  }

        private:
            /**
             * @brief This element is needed, otherwise an empty struct won't compile (don't know exactly why).
             */
            short foo{};
    };


    struct DisabledForRender {
        explicit DisabledForRender(Node* _node, Manager* _manager, Graph* _graph) {  }

        private:
            /**
             * @brief This element is needed, otherwise an empty struct won't compile (don't know exactly why).
             */
            short foo{};
    };

    struct DisabledForEvent {
        explicit DisabledForEvent(Node* _node, Manager* _manager, Graph* _graph) {  }

        private:
            /**
             * @brief This element is needed, otherwise an empty struct won't compile (don't know exactly why).
             */
            short foo{};
    };

    struct DisabledForUpdate {
        explicit DisabledForUpdate(Node* _node, Manager* _manager, Graph* _graph) {  }

        private:
            /**
             * @brief This element is needed, otherwise an empty struct won't compile (don't know exactly why).
             */
            short foo{};
    };

    struct DisabledForFixedUpdate {
        explicit DisabledForFixedUpdate(Node* _node, Manager* _manager, Graph* _graph) {  }

        private:
            /**
             * @brief This element is needed, otherwise an empty struct won't compile (don't know exactly why).
             */
            short foo{};
    };

    struct DisabledForLateUpdate {
        explicit DisabledForLateUpdate(Node* _node, Manager* _manager, Graph* _graph) {  }

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
        explicit CanvasEventStopper(Node* _node, Manager* _manager, Graph* _graph) {  }

        void setEnabled(bool _enabled) override {
            enabled = _enabled;
        }

        bool isEnabled() override {
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
