//
// Created by borja on 9/16/22.
//

#ifndef GDE_PHYSICS_COLLIDER_H
#define GDE_PHYSICS_COLLIDER_H


#include "PhysicsManifold.h"

namespace GDE {

    typedef void (*CollisionCallback)( PhysicsManifold *m, PhysicsBody *a, PhysicsBody *b );

    extern CollisionCallback Dispatch[PhysicsShape::MAX][PhysicsShape::MAX];

    void circleToCircle(PhysicsManifold* _manifold, PhysicsBody* _bodyA, PhysicsBody* _bodyB);
    void circleToPolygon(PhysicsManifold* _manifold, PhysicsBody* _bodyA, PhysicsBody* _bodyB);
    void polygonToCircle(PhysicsManifold* _manifold, PhysicsBody* _bodyA, PhysicsBody* _bodyB);
    void polygonToPolygon(PhysicsManifold* _manifold, PhysicsBody* _bodyA, PhysicsBody* _bodyB);

}


#endif //GDE_PHYSICS_COLLIDER_H
