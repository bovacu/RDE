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
        contacts.clear();
        for(auto _i = 0; _i < bodies.size(); _i++) {
            PhysicsBody* _a = bodies[_i];

            for(auto _j = _i + 1; _j < bodies.size(); _j++) {
                PhysicsBody* _b = bodies[_j];

                if(_a->inverseMass == 0 && _b->inverseMass == 0)
                    continue;

                PhysicsManifold _m(_a, _b );

                _m.solve();
                if(_m.contactCount)
                    contacts.emplace_back(_m);
            }
        }

        // Integrate forces
        for(auto* _body : bodies)
            integrateForces(_body, _fxDt );

        // Initialize collision
        for(auto& _contact : contacts)
            _contact.initialize(_fxDt, gravity);

        // Solve collisions
        for(auto _i = 0; _i < steps; _i++)
            for(auto& _contact : contacts)
                _contact.applyImpulse();

        // Integrate velocities
        for(auto* _body : bodies)
            integrateVelocity(_body, _fxDt);

        // Correct positions
        for(auto& _contact : contacts)
            _contact.positionalCorrection();

        // Clear all forces
        for(auto* _body : bodies) {
            _body->force.set( 0, 0 );
            _body->torque = 0;
        }
    }

    void PhysicsManager::integrateForces(PhysicsBody* _physicsBody, Delta _fixedDelta) {
        if(_physicsBody->inverseMass == 0.0f)
            return;

        _physicsBody->velocity += (_physicsBody->force * _physicsBody->inverseMass + gravity) * (_fixedDelta / 2.0f);
        _physicsBody->angularVelocity += _physicsBody->torque * _physicsBody->inverseInertia * (_fixedDelta / 2.0f);
    }

    void PhysicsManager::integrateVelocity(PhysicsBody* _physicsBody, Delta _fxDt) {
        if(_physicsBody->inverseMass == 0.0f)
            return;

        _physicsBody->position += _physicsBody->velocity * _fxDt;

        _physicsBody->rotation += radiansToDegrees(_physicsBody->angularVelocity * _fxDt);
        _physicsBody->rotate(_physicsBody->rotation);
        integrateForces(_physicsBody, _fxDt);
    }

    void PhysicsManager::debugRender(RenderManager* _renderManager) {
        for(auto* _body : bodies) {

            switch (_body->shape->type) {
                case PhysicsShape::Type::CIRCLE: {

                    const int _segments = 20;
                    float _theta = _body->rotation;
                    float _inc = PI * 2.0f / (float)_segments;
                    Vec2F _points[_segments];

                    for(auto& _point : _points) {
                        _theta += _inc;
                        Vec2F _p(std::cos(_theta), std::sin(_theta) );
                        _p *= _body->shape->size.x;
                        _p += _body->position;
                        _point = _p;
                    }

                    for (auto _i = 0; _i < _segments; _i++) {
                        int _next = _i + 1;
                        if(_next == _segments)
                            _next = 0;

                        Vec2F _p0 = _points[_i];
                        Vec2F _p1 =_points[_next];

                        _renderManager->drawLine(_p0, _p1, Color::Blue);
                    }

                    break;
                }

                case PhysicsShape::Type::POLYGON: {
                    auto* _polygon = _body->shape;
                    for (auto _i = 0; _i < _polygon->vertices.size(); _i++) {

                        int _next = _i + 1;
                        if(_next == _polygon->vertices.size())
                            _next = 0;

                        Vec2F _p0 = _body->position + _polygon->u * _polygon->vertices[_i];
                        Vec2F _p1 = _body->position + _polygon->u * _polygon->vertices[_next];

                        _renderManager->drawLine(_p0, _p1, Color::Blue);
                    }

                    break;
                }
                default:
                    break;
            }
        }
    }

    PhysicsBody* PhysicsManager::add(PhysicsShape* _physicsShape, const Vec2F& _position) {
        ENGINE_ASSERT(_physicsShape, "Cannot add a NULLPTR physics body to the simulation!!");
        auto* _physicsBody = new PhysicsBody( _physicsShape, _position );
        bodies.push_back(_physicsBody);
        return _physicsBody;
    }
}
#pragma clang diagnostic pop