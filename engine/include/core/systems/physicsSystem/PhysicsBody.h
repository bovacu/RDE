//
// Created by borja on 9/16/22.
//

#ifndef GDE_PHYSICS_BODY_H
#define GDE_PHYSICS_BODY_H


#include "core/render/shapes/DebugShape.h"
#include "PhysicsMath.h"
#include "PhysicsShape.h"

namespace GDE {

    struct PhysicsBody {

        public:
            PhysicsBody(PhysicsShape* _shape, const Vec2F& _position);

            void applyForce(const Vec2F& _force);
            void applyImpulse(const Vec2F& _impulse, const Vec2F& _contactVector);
            void setStatic();
            void rotate(float _degrees);

            Transform* transform;
            PhysicsShape *shape;

            Vec2F velocity;
            float angularVelocity;
            float torque;

            Vec2F force;

            float inertia;
            float inverseInertia;
            float mass;
            float inverseMass;
            float density;

            float staticFriction;
            float dynamicFriction;
            float restitution;

            /**
             * @brief This is the "ID" of the body, it tells which type of body it is. It must be a 2^n.
             */
            ulong collisionMask;

        private:
            void computeMass(PhysicsShape* _shape);
            void computePolygonMass(PhysicsShape* _shape);
            void computeCircleMass(PhysicsShape* _shape);
    };

}


#endif //GDE_PHYSICS_BODY_H
