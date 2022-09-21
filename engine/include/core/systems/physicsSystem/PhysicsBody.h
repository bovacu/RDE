//
// Created by borja on 9/16/22.
//

#ifndef GDE_PHYSICS_BODY_H
#define GDE_PHYSICS_BODY_H


#include "core/render/shapes/DebugShape.h"
#include "core/systems/physicsSystem/PhysicsMath.h"
#include "core/systems/physicsSystem/PhysicsShape.h"

namespace GDE {

    struct ShapeConfig {
        PhysicsShape::Type type = PhysicsShape::POLYGON;
        Vec2F size = { 32, 32 };
        std::vector<Vec2F> vertices {  };
    };

    struct BodyConfig {
        ShapeConfig shapeConfig {  };
        bool isStatic = false;
        float dynamicFriction = 0.3f;
        float staticFriction = 0.5f;
        float restitution = 0.2f;
        float density = 1.0f;
        bool ghost = false;
        ulong collisionMask = 0;
    };

    struct PhysicsBody {

        public:
            PhysicsBody(const NodeID& _id, Transform* _transform, const BodyConfig& _bodyConfig);

            void applyForce(const Vec2F& _force);
            void applyImpulse(const Vec2F& _impulse, const Vec2F& _contactVector);
            void setStatic();
            void rotate(float _degrees);

            Transform* transform;
            PhysicsShape shape;

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

            bool ghost;

            /**
             * @brief This is the "ID" of the body, it tells which type of body it is. It must be a 2^n.
             */
            ulong collisionMask;

        private:
            void computeMass(PhysicsShape& _shape);
            void computePolygonMass(PhysicsShape& _shape);
            void computeCircleMass(PhysicsShape& _shape);
    };

}


#endif //GDE_PHYSICS_BODY_H
