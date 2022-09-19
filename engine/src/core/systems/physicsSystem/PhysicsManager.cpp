//
// Created by borja on 22/04/22.
//

#include "core/systems/physicsSystem/PhysicsManager.h"
#include "core/render/RenderManager.h"
#include "core/physics/Precompiled.h"

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

    void IntegrateForces( Physics::Body *b, real _dt )
    {
        if(b->im == 0.0f)
            return;

        b->velocity += (b->force * b->im + gravity) * (_dt / 2.0f);
        b->angularVelocity += b->torque * b->iI * (_dt / 2.0f);
    }

    void IntegrateVelocity( Physics::Body *b, real _dt )
    {
        if(b->im == 0.0f)
            return;

        b->position += b->velocity * _dt;
        b->orient += b->angularVelocity * _dt;
        b->SetOrient( b->orient );
        IntegrateForces( b, _dt );
    }

    void PhysicsManager::step(Delta _fxDt) {
        contacts.clear( );
        for(uint32 i = 0; i < bodies.size( ); ++i)
        {
            Physics::Body *A = bodies[i];

            for(uint32 j = i + 1; j < bodies.size( ); ++j)
            {
                Physics::Body *B = bodies[j];
                if(A->im == 0 && B->im == 0)
                    continue;
                Physics::Manifold m( A, B );
                m.Solve( );
                if(m.contact_count)
                    contacts.emplace_back( m );
            }
        }

        // Integrate forces
        for(uint32 i = 0; i < bodies.size( ); ++i)
            IntegrateForces( bodies[i], _fxDt );

        // Initialize collision
        for(uint32 i = 0; i < contacts.size( ); ++i)
            contacts[i].Initialize( );

        // Solve collisions
        for(uint32 j = 0; j < m_iterations; ++j)
            for(uint32 i = 0; i < contacts.size( ); ++i)
                contacts[i].ApplyImpulse( );

        // Integrate velocities
        for(uint32 i = 0; i < bodies.size( ); ++i)
            IntegrateVelocity( bodies[i], _fxDt );

        // Correct positions
        for(uint32 i = 0; i < contacts.size( ); ++i)
            contacts[i].PositionalCorrection( );

        // Clear all forces
        for(uint32 i = 0; i < bodies.size( ); ++i)
        {
            Physics::Body *b = bodies[i];
            b->force.Set( 0, 0 );
            b->torque = 0;
        }
    }

//    void PhysicsManager::integrateForces(PhysicsBody* b, Delta _fixedDelta) {
//        if(b->im == 0.0f)
//            return;
//
//        auto _partialVelocity = (b->force * b->im + gravity);
//        auto _deltaHalf = (_fixedDelta / 2.0f);
//        auto _newVelocity = _partialVelocity * _deltaHalf;
//        b->velocity = b->velocity + _newVelocity;
//
//        auto _partialAngularVelocity = b->torque * b->iI;
//        auto _newAngularVelocity = _partialAngularVelocity * _deltaHalf;
//        b->angularVelocity = b->angularVelocity + _newAngularVelocity;
//    }

//    void PhysicsManager::integrateVelocity(PhysicsBody* b, Delta _fxDt) {
//        if(b->im == 0.0f)
//            return;
//
//        auto _newPos = b->velocity * _fxDt;
//        b->position = b->position + _newPos;
//        auto _newAngle = b->angularVelocity * _fxDt;
//        b->orient = b->orient + _newAngle;
//        b->setOrient(b->orient);
//        integrateForces(b, _fxDt);
//        integrateForces(b, _fxDt);
//    }

    void PhysicsManager::debugRender(RenderManager* _renderManager) {
        for(auto* _body : bodies)
        {
            switch (_body->shape->GetType()) {

                case Physics::Shape::eCircle:
                {
                    const uint32 k_segments = 20;
                    real theta = _body->orient;
                    real inc = PI * 2.0f / (real)k_segments;
                    Physics::Vec2 _points[k_segments];
                    for(uint32 i = 0; i < k_segments; ++i)
                    {
                        theta += inc;
                        Physics::Vec2 p( std::cos( theta ), std::sin( theta ) );
                        p *= _body->shape->radius;
                        p += _body->position;
                        _points[i] = p;
                    }

                    for (auto _i = 0; _i < k_segments; _i++) {

                        int _next = _i + 1;
                        if(_next == k_segments)
                            _next = 0;

                        Physics::Vec2 _p0_ = _points[_i];
                        Physics::Vec2 _p1_ =_points[_next];

                        Vec2F _p0 { _p0_.x, _p0_.y };
                        Vec2F _p1 { _p1_.x, _p1_.y };
                        _renderManager->drawLine(_p0, _p1, Color::Blue);
                    }

                    break;
                }
                case Physics::Shape::ePoly:
                {
                    auto* _polygon = dynamic_cast<Physics::PolygonShape*>(_body->shape);
                    for (auto _i = 0; _i < _polygon->m_vertices.size(); _i++) {

                        int _next = _i + 1;
                        if(_next == _polygon->m_vertices.size())
                            _next = 0;

                        Physics::Vec2 _p0_ = _body->position + _polygon->u * _polygon->m_vertices[_i];
                        Physics::Vec2 _p1_ = _body->position + _polygon->u * _polygon->m_vertices[_next];

                        Vec2F _p0 { _p0_.x, _p0_.y };
                        Vec2F _p1 { _p1_.x, _p1_.y };
                        _renderManager->drawLine(_p0, _p1, Color::Blue);
                    }

                    break;
                }
                default:
                    break;
            }
        }
    }

    Physics::Body* PhysicsManager::add(Physics::Shape* _physicsShape, const Vec2F& _position) {
        assert( _physicsShape );
        auto *b = new Physics::Body( _physicsShape, _position.x, _position.y );
        bodies.push_back( b );
        return b;
    }
}
#pragma clang diagnostic pop