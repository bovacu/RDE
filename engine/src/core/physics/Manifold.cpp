/*
    Copyright (c) 2013 Randy Gaul http://RandyGaul.net

    This software is provided 'as-is', without any express or implied
    warranty. In no event will the authors be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:
      1. The origin of this software must not be misrepresented; you must not
         claim that you wrote the original software. If you use this software
         in a product, an acknowledgment in the product documentation would be
         appreciated but is not required.
      2. Altered source versions must be plainly marked as such, and must not be
         misrepresented as being the original software.
      3. This notice may not be removed or altered from any source distribution.
*/

#include "core/physics/Precompiled.h"
#include "core/physics/Manifold.h"
#include "core/systems/physicsSystem/PhysicsMath.h"

namespace Physics {

    void Manifold::Solve(void) {
        Dispatch[A->shape->GetType()][B->shape->GetType()](this, A, B);
    }

    void Manifold::Initialize(GDE::Delta _fxDt, const GDE::Vec2F& _gravity) {
        // Calculate average restitution
        e = std::min(A->restitution, B->restitution);

        // Calculate static and dynamic friction
        sf = std::sqrt(A->staticFriction * B->staticFriction);
        df = std::sqrt(A->dynamicFriction * B->dynamicFriction);

        for (uint32_t i = 0; i < contact_count; ++i) {
            // Calculate radii from COM to contact
            GDE::Vec2F ra = contacts[i] - A->position;
            GDE::Vec2F rb = contacts[i] - B->position;

            GDE::Vec2F rv = B->velocity + rb.crossProduct(B->angularVelocity) -
                            A->velocity - ra.crossProduct(A->angularVelocity);


            // Determine if we should perform a resting collision or not
            // The idea is if the only thing moving this object is gravity,
            // then the collision should be performed without any restitution
            if (rv.magnitudeSqr() < (_fxDt * _gravity).magnitudeSqr() + EPSILON)
                e = 0.0f;
        }
    }

    void Manifold::ApplyImpulse(void) {
        // Early out and positional correct if both objects have infinite mass
        if (GDE::PhysicsMath::approximatelyEqual(A->im + B->im, 0)) {
            InfiniteMassCorrection();
            return;
        }

        for (uint32_t i = 0; i < contact_count; ++i) {
            // Calculate radii from COM to contact
            GDE::Vec2F ra = contacts[i] - A->position;
            GDE::Vec2F rb = contacts[i] - B->position;

            // Relative velocity
            GDE::Vec2F rv = B->velocity + rb.crossProduct(B->angularVelocity) -
                            A->velocity - ra.crossProduct(A->angularVelocity);

            // Relative velocity along the normal
            float contactVel = rv.dotProduct(normal);

            // Do not resolve if velocities are separating
            if (contactVel > 0)
                return;

            float raCrossN = ra.crossProduct(normal);
            float rbCrossN = rb.crossProduct(normal);
            float invMassSum = A->im + B->im + (raCrossN * raCrossN) * A->iI + (rbCrossN * rbCrossN) * B->iI;

            // Calculate impulse scalar
            float j = -(1.0f + e) * contactVel;
            j /= invMassSum;
            j /= (float) contact_count;

            // Apply impulse
            GDE::Vec2F impulse = normal * j;
            A->ApplyImpulse(-impulse, ra);
            B->ApplyImpulse(impulse, rb);

            // Friction impulse
            rv = B->velocity + rb.crossProduct(B->angularVelocity) -
                 A->velocity - ra.crossProduct(A->angularVelocity);

            GDE::Vec2F t = rv - (normal * rv.dotProduct(normal));
            t.normalize();

            // j tangent magnitude
            float jt = -rv.dotProduct(t);
            jt /= invMassSum;
            jt /= (float) contact_count;

            // Don't apply tiny friction impulses
            if (GDE::PhysicsMath::approximatelyEqual(jt, 0.0f))
                return;

            // Coulumb's law
            GDE::Vec2F tangentImpulse;
            if (std::abs(jt) < j * sf)
                tangentImpulse = t * jt;
            else
                tangentImpulse = t * -j * df;

            // Apply friction impulse
            A->ApplyImpulse(-tangentImpulse, ra);
            B->ApplyImpulse(tangentImpulse, rb);
        }
    }

    void Manifold::PositionalCorrection(void) {
        const float k_slop = 0.05f; // Penetration allowance
        const float percent = 0.4f; // Penetration percentage to correct
        GDE::Vec2F correction = (std::max(penetration - k_slop, 0.0f) / (A->im + B->im)) * normal * percent;
        A->position -= correction * A->im;
        B->position += correction * B->im;
    }

    void Manifold::InfiniteMassCorrection(void) {
        A->velocity.set(0, 0);
        B->velocity.set(0, 0);
    }


}