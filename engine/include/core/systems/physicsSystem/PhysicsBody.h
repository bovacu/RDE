//
// Created by borja on 9/16/22.
//

#ifndef GDE_PHYSICS_BODY_H
#define GDE_PHYSICS_BODY_H


#include "core/render/shapes/Shape.h"
#include "PhysicsMath.h"
#include "PhysicsShape.h"

namespace GDE {

    struct PhysicsBody {
        PhysicsBody(PhysicsShape* _shape, const Vec2F& _position);

        void applyForce( const Vec2F& f ) {
            force = force + f;
        }

        void applyImpulse( const Vec2F& impulse, const Vec2F& contactVector )
        {
            velocity = velocity + impulse * im;
            angularVelocity = angularVelocity + iI * cross( contactVector, impulse );
        }

        void setStatic( void )
        {
            I = 0.0f;
            iI = 0.0f;
            m = 0.0f;
            im = 0.0f;
        }

        void setOrient(float _degrees);

        Vec2F position;
        Vec2F velocity;

        float angularVelocity;
        float torque;
        float orient; // degrees

        Vec2F force;

        // Set by shape
        float I;  // moment of inertia
        float iI; // inverse inertia
        float m;  // mass
        float im; // inverse masee
        float density;

        // http://gamedev.tutsplus.com/tutorials/implementation/how-to-create-a-custom-2d-physics-engine-friction-scene-and-jump-table/
        float staticFriction;
        float dynamicFriction;
        float restitution;

        // Shape interface
        PhysicsShape *shape;

        // Store a color in RGB format
        float r, g, b;

        private:
            void computeMass(PhysicsShape* _shape);
            void computePolygonMass(PhysicsShape* _shape);
            void computeCircleMass(PhysicsShape* _shape);
    };

}


#endif //GDE_PHYSICS_BODY_H
