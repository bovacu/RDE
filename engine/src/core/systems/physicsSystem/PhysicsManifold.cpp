//
// Created by borja on 9/16/22.
//

#include "core/systems/physicsSystem/PhysicsManifold.h"

namespace GDE {

    void PhysicsManifold::initialize(Delta _fxDt, const Vec2F& _gravity) {
        // Calculate average restitution
        e = std::min(A->restitution, B->restitution);

        // Calculate static and dynamic friction
        sf = std::sqrt(A->staticFriction * B->staticFriction);
        df = std::sqrt(A->dynamicFriction * B->dynamicFriction);

        for (auto _i = 0; _i < contactCount; _i++) {
            // Calculate radii from COM to contact
            Vec2F _ra = contacts[_i] - A->transform->getPosition();
            Vec2F _rb = contacts[_i] - B->transform->getPosition();

            Vec2F _rv = B->velocity + _rb.crossProduct(B->angularVelocity) - A->velocity - _ra.crossProduct(A->angularVelocity);

            // Determine if we should perform a resting collision or not
            // The idea is if the only thing moving this object is gravity,
            // then the collision should be performed without any restitution
            if (_rv.magnitudeSqr() < (_fxDt * _gravity).magnitudeSqr() + EPSILON)
                e = 0.0f;
        }
    }

    void PhysicsManifold::applyImpulse() {
        // Early out and positional correct if both objects have infinite mass
        if (PhysicsMath::approximatelyEqual(A->inverseMass + B->inverseMass, 0)) {
            infiniteMassCorrection();
            return;
        }

        for (auto _i = 0; _i < contactCount; ++_i) {
            // Calculate radii from COM to contact
            Vec2F _ra = contacts[_i] - A->transform->getPosition();
            Vec2F _rb = contacts[_i] - B->transform->getPosition();

            // Relative velocity
            Vec2F _rv = B->velocity + _rb.crossProduct(B->angularVelocity) - A->velocity - _ra.crossProduct(A->angularVelocity);

            // Relative velocity along the normal
            float _contactVel = _rv.dotProduct(normal);

            // Do not resolve if velocities are separating
            if (_contactVel > 0)
                return;

            float _raCrossN = _ra.crossProduct(normal);
            float _rbCrossN = _rb.crossProduct(normal);
            float _invMassSum = A->inverseMass + B->inverseMass + (_raCrossN * _raCrossN) * A->inverseInertia + (_rbCrossN * _rbCrossN) * B->inverseInertia;

            // Calculate _impulse scalar
            float _j = -(1.0f + e) * _contactVel;
            _j /= _invMassSum;
            _j /= (float) contactCount;

            // Apply _impulse
            Vec2F _impulse = normal * _j;
            A->applyImpulse(-_impulse, _ra);
            B->applyImpulse(_impulse, _rb);

            // Friction _impulse
            _rv = B->velocity + _rb.crossProduct(B->angularVelocity) - A->velocity - _ra.crossProduct(A->angularVelocity);

            Vec2F _t = _rv - (normal * _rv.dotProduct(normal));
            _t.normalize();

            // _j tangent magnitude
            float _jt = -_rv.dotProduct(_t);
            _jt /= _invMassSum;
            _jt /= (float) contactCount;

            // Don'_t apply tiny friction impulses
            if (PhysicsMath::approximatelyEqual(_jt, 0.0f))
                return;

            // Coulumb's law
            Vec2F _tangentImpulse;
            if (std::abs(_jt) < _j * sf)
                _tangentImpulse = _t * _jt;
            else
                _tangentImpulse = _t * -_j * df;

            // Apply friction _impulse
            A->applyImpulse(-_tangentImpulse, _ra);
            B->applyImpulse(_tangentImpulse, _rb);
        }
    }

    void PhysicsManifold::positionalCorrection() {
        const float _slop = 0.05f; // Penetration allowance
        const float _percent = 0.4f; // Penetration percentage to correct
        Vec2F _correction = (std::max(penetration - _slop, 0.0f) / (A->inverseMass + B->inverseMass)) * normal * _percent;
        A->transform->translate(-(_correction * A->inverseMass));
        B->transform->translate(_correction * B->inverseMass);
    }

    void PhysicsManifold::infiniteMassCorrection() {
        A->velocity.set(0, 0);
        B->velocity.set(0, 0);
    }
}