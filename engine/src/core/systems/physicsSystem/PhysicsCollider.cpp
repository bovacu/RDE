//
// Created by borja on 9/16/22.
//

#include "core/systems/physicsSystem/PhysicsCollider.h"
#include "core/util/Mat2.h"

namespace GDE {

    CollisionCallback Dispatch[PhysicsShape::Type::MAX][PhysicsShape::Type::MAX] =
    {
        {
            circleToCircle, circleToPolygon
        },
        {
            polygonToCircle, polygonToPolygon
        },
    };

    void circleToCircle(PhysicsManifold* _manifold, PhysicsBody* _bodyA, PhysicsBody* _bodyB) {
        auto* _a = _bodyA->shape;
        auto* _b = _bodyB->shape;

        // Calculate translational vector, which is _normal
        Vec2F _normal = _bodyB->position - _bodyA->position;

        float _distSqr = _normal.magnitudeSqr();
        float _radius = _a->size.x + _b->size.x;

        // Not in contact
        if (_distSqr >= _radius * _radius) {
            _manifold->contactCount = 0;
            return;
        }

        float _distance = std::sqrt(_distSqr);

        _manifold->contactCount = 1;

        if (_distance == 0.0f) {
            _manifold->penetration = _a->size.x;
            _manifold->normal = GDE::Vec2F(1, 0);
            _manifold->contacts[0] = _bodyA->position;
        } else {
            _manifold->penetration = _radius - _distance;
            _manifold->normal = _normal / _distance; // Faster than using Normalized since we already performed sqrt
            _manifold->contacts[0] = _manifold->normal * _a->size.x + _bodyA->position;
        }
    }

    void circleToPolygon(PhysicsManifold* _manifold, PhysicsBody* _bodyA, PhysicsBody* _bodyB) {
        auto* _a = _bodyA->shape;
        auto* _b = _bodyB->shape;

        _manifold->contactCount = 0;

        // Transform circle _center to Polygon model space
        Vec2F _center = _bodyA->position;
        _center = _b->u.transpose() * (_center - _bodyB->position);

        // Find edge with minimum penetration
        // Exact concept as using support points in Polygon vs Polygon
        float _separation = -FLT_MAX;
        auto _faceNormal = 0;
        for (auto _i = 0; _i < _b->vertexCount; ++_i) {
            float _s = _b->normals[_i].dotProduct(_center - _b->vertices[_i]);

            if (_s > _a->size.x)
                return;

            if (_s > _separation) {
                _separation = _s;
                _faceNormal = _i;
            }
        }

        // Grab face's vertices
        Vec2F _v1 = _b->vertices[_faceNormal];
        auto _i2 = _faceNormal + 1 < _b->vertexCount ? _faceNormal + 1 : 0;
        Vec2F _v2 = _b->vertices[_i2];

        // Check to see if _center is within polygon
        if (_separation < EPSILON) {
            _manifold->contactCount = 1;
            _manifold->normal = -(_b->u * _b->normals[_faceNormal]);
            _manifold->contacts[0] = _manifold->normal * _a->size.x + _bodyA->position;
            _manifold->penetration = _a->size.x;
            return;
        }

        // Determine which voronoi region of the edge _center of circle lies within
        float _dot1 = (_center - _v1).dotProduct(_v2 - _v1);
        float _dot2 = (_center - _v2).dotProduct(_v1 - _v2);
        _manifold->penetration = _a->size.x - _separation;

        // Closest to _v1
        if (_dot1 <= 0.0f) {
            if (_center.distanceSqr(_v1) > _a->size.x * _a->size.x)
                return;

            _manifold->contactCount = 1;
            Vec2F _n = _v1 - _center;
            _n = _b->u * _n;
            _n.normalize();
            _manifold->normal = _n;
            _v1 = _b->u * _v1 + _bodyB->position;
            _manifold->contacts[0] = _v1;
        }

            // Closest to _v2
        else if (_dot2 <= 0.0f) {
            if (_center.distance(_v2) > _a->size.x * _a->size.x)
                return;

            _manifold->contactCount = 1;
            Vec2F _n = _v2 - _center;
            _v2 = _b->u * _v2 + _bodyB->position;
            _manifold->contacts[0] = _v2;
            _n = _b->u * _n;
            _n.normalize();
            _manifold->normal = _n;
        }

            // Closest to face
        else {
            Vec2F _n = _b->normals[_faceNormal];
            if ((_center - _v1).dotProduct(_n) > _a->size.x)
                return;

            _n = _b->u * _n;
            _manifold->normal = -_n;
            _manifold->contacts[0] = _manifold->normal * _a->size.x + _bodyA->position;
            _manifold->contactCount = 1;
        }
    }

    void polygonToCircle(PhysicsManifold* _manifold, PhysicsBody* _bodyA, PhysicsBody* _bodyB) {
        circleToPolygon(_manifold, _bodyB, _bodyA);
        _manifold->normal = -_manifold->normal;
    }

    float findAxisLeastPenetration(uint32_t* _faceIndex, PhysicsShape* _shapeA, PhysicsShape* _shapeB) {
        float _bestDistance = -FLT_MAX;
        uint32_t _bestIndex;

        for (auto _i = 0; _i < _shapeA->vertexCount; ++_i) {
            // Retrieve a face normal from _shapeA
            Vec2F _n = _shapeA->normals[_i];
            Vec2F _nw = _shapeA->u * _n;

            // Transform face normal into _shapeB'_s model space
            Mat2 _buT = _shapeB->u.transpose();
            _n = _buT * _nw;

            // Retrieve support point from _shapeB along -_n
            Vec2F _s = _shapeB->getSupport(-_n);

            // Retrieve vertex on face from _shapeA, transform into
            // _shapeB'_s model space
            Vec2F _v = _shapeA->vertices[_i];
            _v = _shapeA->u * _v + _shapeA->physicsBody->position;
            _v -= _shapeB->physicsBody->position;
            _v = _buT * _v;

            // Compute penetration distance (in _shapeB'_s model space)
            float _d = _n.dotProduct(_s - _v);

            // Store greatest distance
            if (_d > _bestDistance) {
                _bestDistance = _d;
                _bestIndex = _i;
            }
        }

        *_faceIndex = _bestIndex;
        return _bestDistance;
    }

    void findIncidentFace(Vec2F* _vec, PhysicsShape* _refPoly, PhysicsShape* _incPoly, uint32_t _referenceIndex) {
        Vec2F _referenceNormal = _refPoly->normals[_referenceIndex];

        // Calculate normal in incident's frame of reference
        _referenceNormal = _refPoly->u * _referenceNormal; // To world space
        _referenceNormal = _incPoly->u.transpose() * _referenceNormal; // To incident's model space

        // Find most anti-normal face on incident polygon
        auto _incidentFace = 0;
        float _minDot = FLT_MAX;
        for (auto _i = 0; _i < _incPoly->vertexCount; ++_i) {
            float _dot = _referenceNormal.dotProduct(_incPoly->normals[_i]);
            if (_dot < _minDot) {
                _minDot = _dot;
                _incidentFace = _i;
            }
        }

        // Assign face vertices for _incidentFace
        _vec[0] = _incPoly->u * _incPoly->vertices[_incidentFace] + _incPoly->physicsBody->position;
        _incidentFace = _incidentFace + 1 >= (int32_t) _incPoly->vertexCount ? 0 : _incidentFace + 1;
        _vec[1] = _incPoly->u * _incPoly->vertices[_incidentFace] + _incPoly->physicsBody->position;
    }

    int clip(Vec2F _n, float _c, Vec2F* _face) {
        auto _sp = 0;
        Vec2F _out[2] = { _face[0],_face[1] };

        // Retrieve distances from each endpoint to the line
        // d = ax + by - _c
        float _d1 = _n.dotProduct(_face[0]) - _c;
        float _d2 = _n.dotProduct(_face[1]) - _c;

        // If negative (behind plane) clip
        if (_d1 <= 0.0f) _out[_sp++] = _face[0];
        if (_d2 <= 0.0f) _out[_sp++] = _face[1];

        // If the points are on different sides of the plane
        if (_d1 * _d2 < 0.0f) {  // less than to ignore -0.0f
            // Push interesection point
            float _alpha = _d1 / (_d1 - _d2);
            _out[_sp] = _face[0] + _alpha * (_face[1] - _face[0]);
            ++_sp;
        }

        // Assign our new converted values
        _face[0] = _out[0];
        _face[1] = _out[1];

        assert(_sp != 3);

        return _sp;
    }

    void polygonToPolygon(PhysicsManifold* _manifold, PhysicsBody* _bodyA, PhysicsBody* _bodyB) {
        auto* _a = _bodyA->shape;
        auto* _b = _bodyB->shape;
        _manifold->contactCount = 0;

        // Check for _bodyA separating axis with _a's face planes
        uint32_t _faceA;
        float _penetrationA = findAxisLeastPenetration(&_faceA, _a, _b);
        if (_penetrationA >= 0.0f)
            return;

        // Check for _bodyA separating axis with _b's face planes
        uint32_t _faceB;
        float _penetrationB = findAxisLeastPenetration(&_faceB, _b, _a);
        if (_penetrationB >= 0.0f)
            return;

        uint32_t _referenceIndex;
        bool _flip; // Always point from _bodyA to _bodyB

        PhysicsShape* _refPoly; // Reference
        PhysicsShape* _incPoly; // Incident

        // Determine which shape contains reference face
        if (PhysicsMath::biasGreaterThan(_penetrationA, _penetrationB)) {
            _refPoly = _a;
            _incPoly = _b;
            _referenceIndex = _faceA;
            _flip = false;
        } else {
            _refPoly = _b;
            _incPoly = _a;
            _referenceIndex = _faceB;
            _flip = true;
        }

        // World space incident face
        Vec2F _incidentFace[2];
        findIncidentFace(_incidentFace, _refPoly, _incPoly, _referenceIndex);

        //        y
        //        ^  ->n       ^
        //      +---c ------posPlane--
        //  x < | i |\
        //      +---+ c-----negPlane--
        //             \       v
        //              r
        //
        //  r : reference face
        //  i : incident poly
        //  c : clipped point
        //  n : incident normal

        // Setup reference face vertices
        Vec2F _v1 = _refPoly->vertices[_referenceIndex];
        _referenceIndex = _referenceIndex + 1 == _refPoly->vertexCount ? 0 : _referenceIndex + 1;
        Vec2F _v2 = _refPoly->vertices[_referenceIndex];

        // Transform vertices to world space
        _v1 = _refPoly->u * _v1 + _refPoly->physicsBody->position;
        _v2 = _refPoly->u * _v2 + _refPoly->physicsBody->position;

        // Calculate reference face side normal in world space
        Vec2F _sidePlaneNormal = (_v2 - _v1);
        _sidePlaneNormal.normalize();

        // Orthogonalize
        Vec2F _refFaceNormal(_sidePlaneNormal.y, -_sidePlaneNormal.x);

        // ax + by = c
        // c is distance from origin
        float _refC = _refFaceNormal.dotProduct(_v1);
        float _negSide = -_sidePlaneNormal.dotProduct(_v1);
        float _posSide = _sidePlaneNormal.dotProduct(_v2);

        // Clip incident face to reference face side planes
        if (clip(-_sidePlaneNormal, _negSide, _incidentFace) < 2)
            return; // Due to floating point error, possible to not have required points

        if (clip(_sidePlaneNormal, _posSide, _incidentFace) < 2)
            return; // Due to floating point error, possible to not have required points

        // Flip
        _manifold->normal = _flip ? -_refFaceNormal : _refFaceNormal;

        // Keep points behind reference face
        auto _cp = 0; // clipped points behind reference face
        float _separation = _refFaceNormal.dotProduct(_incidentFace[0]) - _refC;
        if (_separation <= 0.0f) {
            _manifold->contacts[_cp] = _incidentFace[0];
            _manifold->penetration = -_separation;
            ++_cp;
        } else
            _manifold->penetration = 0;

        _separation = _refFaceNormal.dotProduct(_incidentFace[1]) - _refC;
        if (_separation <= 0.0f) {
            _manifold->contacts[_cp] = _incidentFace[1];

            _manifold->penetration += -_separation;
            ++_cp;

            // Average penetration
            _manifold->penetration /= (float) _cp;
        }

        _manifold->contactCount = _cp;
    }
}