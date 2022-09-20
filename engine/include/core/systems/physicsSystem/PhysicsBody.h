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
        PhysicsBody(PhysicsShape* _shape, const Vec2F& _position);

        void applyForce(const Vec2F& _force);
        void applyImpulse(const Vec2F& _impulse, const Vec2F& _contactVector);
        void setStatic();
        void rotate(float _degrees);

        Vec2F position;
        Vec2F velocity;

        float angularVelocity;
        float torque;
        float rotation; // radians

        Vec2F force;

        // Set by shape
        float inertia;  // moment of inertia
        float inverseInertia; // inverse inertia
        float mass;  // mass
        float inverseMass; // inverse masee
        float density;

        // http://gamedev.tutsplus.com/tutorials/implementation/how-to-create-a-custom-2d-physics-engine-friction-scene-and-jump-table/
        float staticFriction;
        float dynamicFriction;
        float restitution;

        // Shape interface
        PhysicsShape *shape;

        private:
            void computeMass(PhysicsShape* _shape);
            void computePolygonMass(PhysicsShape* _shape);
            void computeCircleMass(PhysicsShape* _shape);
    };

}


#endif //GDE_PHYSICS_BODY_H
