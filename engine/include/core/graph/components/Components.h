// Created by borja on 27/12/21.


#ifndef ENGINE2_0_COMPONENTS_H
#define ENGINE2_0_COMPONENTS_H

#include "core/util/Util.h"
#include "core/render/elements/ShaderManager.h"
#include "core/graph/Graph.h"
#include "glm/ext/matrix_transform.hpp"
#include "core/systems/uiSystem/FontManager.h"
#include "core/render/elements/IRenderizable.h"
#include "box2d/box2d.h"
#include "core/systems/physicsSystem/PhysicsManager.h"
#include "core/systems/inputSystem/keysAndButtons/KeyboardKeys.h"
#include "core/systems/inputSystem/keysAndButtons/MouseKeys.h"
#include "core/systems/inputSystem/keysAndButtons/ControllerButtons.h"

namespace GDE {

    class Manager; class Window; class Camera; class Canvas; class IViewPort;

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
        explicit Active(const NodeID& _nodeId) {  }

        private:
            /**
             * @brief This element is needed, otherwise an empty struct won't compile (don't know exactly why).
             */
            short foo;
    };

    /**
     * @brief Component that helps the internal system work quickier. This component should be added to entities that won't move at all during
     * the scene rendering and life cycle.
     *
     */
    struct StaticTransform {
        explicit StaticTransform(const NodeID& _nodeId) {  }

        private:
            /**
             * @brief This element is needed, otherwise an empty struct won't compile (don't know exactly why).
             */
            short foo;
    };

    /**
     * @brief Component used to block the event that the entity containing this component handled. If an entity
     * handles an event without this component, the event will propagate in deeper elements.
     * 
     */
    struct CanvasEventStopper {
        explicit CanvasEventStopper(const NodeID& _nodeId) {  }

        private:
            /**
             * @brief This element is needed, otherwise an empty struct won't compile (don't know exactly why).
             */
            short foo;
    };
}

#endif //ENGINE2_0_COMPONENTS_H
