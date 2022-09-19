//
// Created by borja on 22/04/22.
//

#include "core/systems/physicsSystem/PhysicsManager.h"
#include "core/render/RenderManager.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "NullDereference"
namespace GDE {

    void PhysicsManager::init() {

    }

    void PhysicsManager::destroy() {
        for(auto* _body : bodies) {
            delete _body->shape;
            delete _body;
        }
    }

    void PhysicsManager::step(Delta _fxDt) {
        // Generate new collision info
        contacts.clear( );
        for(uint32 i = 0; i < bodies.size( ); ++i)
        {
            PhysicsBody *A = bodies[i];

            for(uint32 j = i + 1; j < bodies.size( ); ++j)
            {
                PhysicsBody *B = bodies[j];
                if(A->im == 0 && B->im == 0)
                    continue;
                PhysicsManifold m( A, B );
                m.solve();
                if(m.contact_count)
                    contacts.emplace_back( m );
            }
        }

        // Integrate forces
        for(auto* body : bodies)
            integrateForces( body, _fxDt );

        // Initialize collision
        for(auto& contact : contacts)
            contact.initialize(_fxDt);

        // Solve collisions
        for(uint32 j = 0; j < m_iterations; ++j)
            for(auto& contact : contacts)
                contact.applyImpulse();

        // Integrate velocities
        for(auto* body : bodies)
            integrateVelocity(body, _fxDt);

        // Correct positions
        for(auto & contact : contacts)
            contact.positionalCorrection();

        // Clear all forces
        for(auto* b : bodies)
        {
            b->force = { 0.0f, 0.0f };
            b->torque = 0;
        }
    }

    void PhysicsManager::integrateForces(PhysicsBody* b, Delta _fixedDelta) {
        if(b->im == 0.0f)
            return;

        auto _partialVelocity = (b->force * b->im + gravity);
        auto _deltaHalf = (_fixedDelta / 2.0f);
        auto _newVelocity = _partialVelocity * _deltaHalf;
        b->velocity = b->velocity + _newVelocity;

        auto _partialAngularVelocity = b->torque * b->iI;
        auto _newAngularVelocity = _partialAngularVelocity * _deltaHalf;
        b->angularVelocity = b->angularVelocity + _newAngularVelocity;
    }

    void PhysicsManager::integrateVelocity(PhysicsBody* b, Delta _fxDt) {
        if(b->im == 0.0f)
            return;

        auto _newPos = b->velocity * _fxDt;
        b->position = b->position + _newPos;
        auto _newAngle = b->angularVelocity * _fxDt;
        b->orient = b->orient + _newAngle;
        b->setOrient(b->orient);
        integrateForces(b, _fxDt);
        integrateForces(b, _fxDt);
    }

    void PhysicsManager::debugRender(RenderManager* _renderManager) {
        for(auto b : bodies)
        {
            auto _shape = b->shape->debugDraw();
            _renderManager->drawShape(_shape);
        }
    }

    PhysicsBody* PhysicsManager::add(PhysicsShape* _physicsShape, const Vec2F& _position) {
        auto *b = new PhysicsBody( _physicsShape, _position );
        bodies.push_back( b );
        return b;
    }
}
#pragma clang diagnostic pop