//
// Created by borja on 9/16/22.
//

#include "core/systems/physicsSystem/PhysicsManifold.h"
#include "core/systems/physicsSystem/PhysicsCollider.h"

namespace GDE {

//    void PhysicsManifold::solve() {
//        Dispatch[A->shape->type][B->shape->type]( this, A, B );
//    }
//
//    void PhysicsManifold::initialize(Delta _fxDt) {
//        // Calculate average restitution
//        e = std::min( A->restitution, B->restitution );
//
//        // Calculate static and dynamic friction
//        sf = std::sqrt( A->staticFriction * B->staticFriction );
//        df = std::sqrt( A->dynamicFriction * B->dynamicFriction );
//
//        for(uint32 i = 0; i < contact_count; ++i)
//        {
//            // Calculate radii from COM to contact
//            Vec2 ra = contacts[i] - A->position;
//            Vec2 rb = contacts[i] - B->position;
//
//            Vec2 rv = B->velocity + cross( B->angularVelocity, rb ) -
//                      A->velocity - cross( A->angularVelocity, ra );
//
//
//            // Determine if we should perform a resting collision or not
//            // The idea is if the only thing moving this object is gravity,
//            // then the collision should be performed without any restitution
//            if(rv.magnitudeSqr( ) < (_fxDt * gravity).magnitudeSqr( ) + EPSILON)
//                e = 0.0f;
//        }
//    }
//
//    void PhysicsManifold::applyImpulse() {
//        // Early out and positional correct if both objects have infinite mass
//        if(equal( A->im + B->im, 0 ))
//        {
//            infiniteMassCorrection( );
//            return;
//        }
//
//        for(uint32 i = 0; i < contact_count; ++i)
//        {
//            // Calculate radii from COM to contact
//            Vec2 ra = contacts[i] - A->position;
//            Vec2 rb = contacts[i] - B->position;
//
//            // Relative velocity
//            Vec2 rv = B->velocity + cross( B->angularVelocity, rb ) -
//                      A->velocity - cross( A->angularVelocity, ra );
//
//            // Relative velocity along the normal
//            real contactVel = dot( rv, normal );
//
//            // Do not resolve if velocities are separating
//            if(contactVel > 0)
//                return;
//
//            real raCrossN = cross( ra, normal );
//            real rbCrossN = cross( rb, normal );
//            real invMassSum = A->im + B->im + sqr( raCrossN ) * A->iI + sqr( rbCrossN ) * B->iI;
//
//            // Calculate impulse scalar
//            real j = -(1.0f + e) * contactVel;
//            j /= invMassSum;
//            j /= (real)contact_count;
//
//            // Apply impulse
//            Vec2 impulse = normal * j;
//            A->applyImpulse( { -impulse.x, -impulse.y }, ra );
//            B->applyImpulse(  impulse, rb );
//
//            // Friction impulse
//            rv = B->velocity + cross( B->angularVelocity, rb ) -
//                 A->velocity - cross( A->angularVelocity, ra );
//
//            Vec2 t = rv - (normal * dot( rv, normal ));
//            t.normalize( );
//
//            // j tangent magnitude
//            real jt = -dot( rv, t );
//            jt /= invMassSum;
//            jt /= (real)contact_count;
//
//            // Don't apply tiny friction impulses
//            if(equal( jt, 0.0f ))
//                return;
//
//            // Coulumb's law
//            Vec2F tangentImpulse;
//            if(std::abs( jt ) < j * sf)
//                tangentImpulse = t * jt;
//            else
//                tangentImpulse = t * -j * df;
//
//            // Apply friction impulse
//            A->applyImpulse( { -tangentImpulse.x, -tangentImpulse.y }, ra );
//            B->applyImpulse(  tangentImpulse, rb );
//        }
//    }
//
//    void PhysicsManifold::positionalCorrection() {
//        const real k_slop = 0.05f; // Penetration allowance
//        const real percent = 0.4f; // Penetration percentage to correct
//        Vec2 correction = (std::max( penetration - k_slop, 0.0f ) / (A->im + B->im)) * normal * percent;
//        A->position = A->position - correction * A->im;
//        B->position = B->position + correction * B->im;
//    }
//
//    void PhysicsManifold::infiniteMassCorrection() {
//        A->velocity.x = 0.0f;
//        A->velocity.y = 0.0f;
//
//        B->velocity.x = 0.0f;
//        B->velocity.y = 0.0f;
//    }
}