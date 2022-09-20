//
// Created by borja on 9/16/22.
//

#ifndef GDE_PHYSICS_MANIFOLD_H
#define GDE_PHYSICS_MANIFOLD_H


#include "PhysicsBody.h"

namespace GDE {

    struct PhysicsManifold {
        PhysicsManifold( PhysicsBody *a, PhysicsBody *b ) : A( a ) , B( b ) {  }

        void initialize(Delta _fxDt, const Vec2F& _gravity);            // Precalculations for impulse solving
        void applyImpulse();          // Solve impulse and apply
        void positionalCorrection();  // Naive correction of positional penetration
        void infiniteMassCorrection();

        PhysicsBody* A;
        PhysicsBody* B;

        float penetration;     // Depth of penetration from collision
        Vec2F normal;          // From A to B
        Vec2F contacts[2];     // Points of contact during collision
        uint32_t contactCount; // Number of contacts that occured during collision
        float e;               // Mixed restitution
        float df;              // Mixed dynamic friction
        float sf;              // Mixed static friction
    };

}


#endif //GDE_PHYSICS_MANIFOLD_H
