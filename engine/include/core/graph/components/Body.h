//
// Created by borja on 9/5/22.
//

#ifndef GDE_BODY_H
#define GDE_BODY_H

#include "core/systems/physicsSystem/PhysicsManager.h"
#include "entt/entity/entity.hpp"
#include "core/graph/Scene.h"

typedef entt::entity NodeID;

namespace GDE {

    /**
     * @brief Component used to give physics properties to an entity. It includes both having gravity and the ability to collide with other elements.
     */
     class Body {
         public:
             /**
              * @see B2DConfig
              */
             B2DConfig b2dConfig;

             /**
              * @see BodyConfig
              */
             BodyConfig bodyConfig;

         public:
             explicit Body(const NodeID& _nodeId, Scene* _scene, const BodyConfig& _config, Transform* _transform);
             Body(const Body& _body) = default;
             ~Body();

             /**
              * @brief Gets the position of the body in WorldSpace.
              *
              * @return Vec2F
              */
             [[nodiscard]] Vec2F getPosition() const;

             /**
              * @brief Gets the rotation of the Body.
              *
              * @return float
              */
             [[nodiscard]] float getRotation() const;

             /**
              * @brief Sets if this body should be affected by gravity or not.
              *
              * @param applyGravity
              */
             void setApplyGravity(bool applyGravity) const;

             /**
              * @brief Returns if the current Body is affected by gravity or not.
              *
              * @return true
              * @return false
              */
             [[nodiscard]] bool isApplyingGravity() const;

             /**
              * @brief Sets a scalar to increase or decrease the gravity that affects this Body.
              *
              * @param _gravityMultiplier
              */
             void setGravityMultiplier(float _gravityMultiplier) const;

             /**
              * @brief Gets the scalar that increases or decreases the gravity of this Body.
              *
              * @return float
              */
             [[nodiscard]] float getGravityMultiplayer() const;

             /**
              * @brief Sets if the collision should just trigger the callback or if also needs to simulate the collision.
              *
              * @param _sensor
              */
             void setSensor(bool _sensor) const;

             /**
              * @brief Returns if the Body just triggers the collision callback.
              *
              * @return true
              * @return false
              */
             [[nodiscard]] bool isSensor() const;

             /**
              * @brief Sets the mask of the Body, so PhysicsManager can understand what kind of Body this is and calculate correctly the collision detection.
              *
              * @param _mask
              */
             void setSelfCollisionMask(CollisionMask _mask) const;

         private:
             /**
              * @brief Conversion from GDE's BodyType to Box2D's BodyType
              *
              * @param _bodyType
              * @return b2BodyType
              */
             b2BodyType gdeBodyTypeToB2dBodyType(const BodyType& _bodyType);
     };

}

#endif //GDE_BODY_H
