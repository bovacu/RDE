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
        orient = random( -PI, PI );
        force = { 0.0f, 0.0f };
        staticFriction = 0.5f;
        dynamicFriction = 0.3f;
        restitution = 0.2f;
        density = 1.0f;
        r = random( 0.2f, 1.0f );
        g = random( 0.2f, 1.0f );
        b = random( 0.2f, 1.0f );

        computeMass(_shape);
    }

    void PhysicsBody::setOrient(float _degrees) {
        orient = _degrees;
        shape->setOrient(_degrees);
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
        Vec2F c { 0.0f, 0.0f }; // centroid
        float area = 0.0f;
        float _inertia = 0.0f;
        const float k_inv3 = 1.0f / 3.0f;

        for(uint32 i1 = 0; i1 < shape->vertexCount; ++i1) {
            // Triangle vertices, third vertex implied as (0, 0)
            Vec2F p1( _shape->vertices[i1] );
            uint32 i2 = i1 + 1 < _shape->vertexCount ? i1 + 1 : 0;
            Vec2F p2( _shape->vertices[i2] );

            float D = cross( p1, p2 );
            float triangleArea = 0.5f * D;

            area += triangleArea;

            // Use area to weight the centroid average, not just vertex position
            c = c + triangleArea * k_inv3 * (p1 + p2);

            real intx2 = p1.x * p1.x + p2.x * p1.x + p2.x * p2.x;
            real inty2 = p1.y * p1.y + p2.y * p1.y + p2.y * p2.y;
            _inertia += (0.25f * k_inv3 * D) * (intx2 + inty2);
        }

        c = c * (1.0f / area);

        // Translate vertices to centroid (make the centroid (0, 0)
        // for the polygon in model space)
        // Not really necessary, but I like doing this anyway
        for(uint32 i = 0; i < _shape->vertexCount; ++i)
            _shape->vertices[i] = _shape->vertices[i] - c;

        m = density * area;
        im = (m) ? 1.0f / m : 0.0f;
        I = _inertia * density;
        iI = I ? 1.0f / I : 0.0f;
    }

    void PhysicsBody::computeCircleMass(PhysicsShape* _shape) {
        m = PI * _shape->size.x * _shape->size.x * density;
        im = (m) ? 1.0f / m : 0.0f;
        I = m * _shape->size.x * _shape->size.x;
        iI = (I) ? 1.0f / I : 0.0f;
    }

}