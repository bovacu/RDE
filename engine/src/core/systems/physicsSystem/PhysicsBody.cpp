//
// Created by borja on 9/16/22.
//

#include "core/systems/physicsSystem/PhysicsBody.h"

namespace GDE {

    PhysicsBody::PhysicsBody(PhysicsShape* _shape, const Vec2F& _position ) : shape(_shape) {
        shape->physicsBody = this;
        position = _position;
        velocity = { 0.0f, 0.0f };
        angularVelocity = 0;
        torque = 0;
        rotation = 0;
        force = { 0.0f, 0.0f };
        staticFriction = 0.5f;
        dynamicFriction = 0.3f;
        restitution = 0.2f;
        density = 1.0f;

        computeMass(_shape);
    }

    void PhysicsBody::rotate(float _degrees) {
        rotation = _degrees;
        shape->rotate(_degrees);
    }

    void PhysicsBody::computeMass(PhysicsShape* _shape) {
        switch (shape->type) {
            case PhysicsShape::CIRCLE:
                computeCircleMass(_shape);
                break;
            case PhysicsShape::POLYGON:
                computePolygonMass(_shape);
                break;
            default:
                break;
        }
    }

    void PhysicsBody::computePolygonMass(PhysicsShape* _shape) {
        // Calculate centroid and moment of interia
        Vec2F _c( 0.0f, 0.0f ); // centroid
        float _area = 0.0f;
        float _inertia = 0.0f;
        const float _inv3 = 1.0f / 3.0f;

        for(auto _i = 0; _i < shape->vertexCount; ++_i) {
            // Triangle vertices, third vertex implied as (0, 0)
            Vec2F _p1( shape->vertices[_i] );
            auto _i2 = _i + 1 < shape->vertexCount ? _i + 1 : 0;
            Vec2F _p2( shape->vertices[_i2] );

            float _d = _p1.crossProduct(_p2);
            float _triangleArea = 0.5f * _d;

            _area += _triangleArea;

            // Use area to weight the centroid average, not just vertex position
            _c += _triangleArea * _inv3 * (_p1 + _p2);

            float _intX2 = _p1.x * _p1.x + _p2.x * _p1.x + _p2.x * _p2.x;
            float _intY2 = _p1.y * _p1.y + _p2.y * _p1.y + _p2.y * _p2.y;
            _inertia += (0.25f * _inv3 * _d) * (_intX2 + _intY2);
        }

        _c *= 1.0f / _area;

        // Translate vertices to centroid (make the centroid (0, 0)
        // for the polygon in model space)
        // Not floatly necessary, but I like doing this anyway
        for(auto _i = 0; _i < shape->vertexCount; _i++)
            shape->vertices[_i] -= _c;

        mass = density * _area;
        inverseMass = (mass) ? 1.0f / mass : 0.0f;
        inertia = _inertia * density;
        inverseInertia = inertia ? 1.0f / inertia : 0.0f;
    }

    void PhysicsBody::computeCircleMass(PhysicsShape* _shape) {
        mass = PI * _shape->size.x * _shape->size.x * density;
        inverseMass = (mass) ? 1.0f / mass : 0.0f;
        inertia = mass * _shape->size.x * _shape->size.x;
        inverseInertia = (inertia) ? 1.0f / inertia : 0.0f;
    }

    void PhysicsBody::setStatic() {
        inertia = 0.0f;
        inverseInertia = 0.0f;
        mass = 0.0f;
        inverseMass = 0.0f;
    }

    void PhysicsBody::applyImpulse(const Vec2F& _impulse, const Vec2F& _contactVector) {
        velocity += inverseMass * _impulse;
        angularVelocity += inverseInertia * _contactVector.crossProduct(_impulse);
    }

    void PhysicsBody::applyForce(const Vec2F& _force) {
        force += _force;
    }

}