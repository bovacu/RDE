//
// Created by borja on 9/16/22.
//

#include <cfloat>
#include "core/systems/physicsSystem/PhysicsShape.h"
#include "core/systems/physicsSystem/PhysicsBody.h"
#include "core/util/Mat2.h"
#include "core/Core.h"
#include "core/util/Logger.h"

namespace GDE {

    void PhysicsShape::makeRectangle(const Vec2F& _size) {
        type = Type::POLYGON;

        vertexCount = 4;
        vertices.emplace_back( -_size.x / 2.f, -_size.y / 2.f );
        vertices.emplace_back(  _size.x / 2.f, -_size.y / 2.f );
        vertices.emplace_back(  _size.x / 2.f,  _size.y / 2.f );
        vertices.emplace_back( -_size.x / 2.f,  _size.y / 2.f );
        normals.emplace_back(  0.0f,  -1.0f );
        normals.emplace_back(  1.0f,   0.0f );
        normals.emplace_back(  0.0f,   1.0f );
        normals.emplace_back( -1.0f,   0.0f );
        size = _size;
    }

    void PhysicsShape::makeCircle(float _radius) {
        size.x = _radius;
        size.y = FLT_MIN;

        type = Type::CIRCLE;
    }

    void PhysicsShape::makePolygon(const std::vector<Vec2F>& _vertices) {

        type = Type::POLYGON;

        // No hulls with less than 3 vertices (ensure actual polygon)
        ENGINE_ASSERT( _vertices.size() > 2 && _vertices.size() <= MAX_VERTICES_PER_POLYGON, "Number of vertices didn't meet requirements")
        auto _count = std::min((int)_vertices.size(), MAX_VERTICES_PER_POLYGON );

        // Find the right most point on the _hull
        int _rightMost = 0;
        float _highestXCoord = _vertices[0].x;
        for(auto _i = 1; _i < _count; ++_i) {
            float _x = _vertices[_i].x;
            if(_x > _highestXCoord) {
                _highestXCoord = _x;
                _rightMost = _i;
            }

                // If matching _x then take farthest negative y
            else if(_x == _highestXCoord)
                if(_vertices[_i].y < _vertices[_rightMost].y)
                    _rightMost = _i;
        }

        int _hull[MAX_VERTICES_PER_POLYGON];
        int _outCount = 0;
        int _indexHull = _rightMost;

        for (;;) {
            _hull[_outCount] = _indexHull;

            // Search for next index that wraps around the _hull
            // by computing cross products to find the most counter-clockwise
            // vertex in the set, given the previous _hull index
            int _nextHullIndex = 0;
            for(int i = 1; i < _count; ++i) {
                // Skip if same coordinate as we need three unique
                // points in the set to perform a cross product
                if(_nextHullIndex == _indexHull) {
                    _nextHullIndex = i;
                    continue;
                }

                // Cross every set of three unique vertices
                // Record each counter clockwise third vertex and add
                // to the output _hull
                // See : http://www.oocities.org/pcgpe/math2d.html
                Vec2F _e1 = _vertices[_nextHullIndex] - _vertices[_hull[_outCount]];
                Vec2F _e2 = _vertices[i] - _vertices[_hull[_outCount]];
                float _c = _e1.crossProduct(_e2);
                if(_c < 0.0f)
                    _nextHullIndex = i;

                // Cross product is zero then e vectors are on same line
                // therefor want to record vertex farthest along that line
                if(_c == 0.0f && _e2.magnitudeSqr( ) > _e1.magnitudeSqr( ))
                    _nextHullIndex = i;
            }

            ++_outCount;
            _indexHull = _nextHullIndex;

            // Conclude algorithm upon wrap-around
            if(_nextHullIndex == _rightMost) {
                vertexCount = _outCount;
                break;
            }
        }

        // Copy vertices into shape's vertices
        for(auto _i = 0; _i < vertexCount; ++_i)
            vertices.push_back(_vertices[_hull[_i]]);

        // Compute face normals
        normals.reserve(vertexCount);
        for(auto _i = 0; _i < vertexCount; ++_i) {
            int _i2 = _i + 1 < vertexCount ? _i + 1 : 0;
            Vec2F _face = vertices[_i2] - vertices[_i];

            // Ensure no zero-length edges, because that's bad
            ENGINE_ASSERT(_face.magnitudeSqr() > EPSILON * EPSILON, "There cannot be zero-length edges!");

            // Calculate normal with 2D cross product between vector and scalar
            normals.emplace_back(Vec2F {_face.y, -_face.x });
            normals.back().normalize();
        }
    }

    Vec2F PhysicsShape::getSupport(const Vec2F& _dir) {
        float _bestProjection = -FLT_MAX;
        Vec2F _bestVertex;

        for(auto _i = 0; _i < vertexCount; ++_i) {
            Vec2 _v = vertices[_i];
            float _projection = _v.dotProduct(_dir);

            if(_projection > _bestProjection) {
                _bestVertex = _v;
                _bestProjection = _projection;
            }
        }

        return _bestVertex;
    }

    void PhysicsShape::rotate(float _degrees) {
        transform->rotate(_degrees);
    }

    Mat2 PhysicsShape::getRotationMatrix() {
        float _c = std::cos(degreesToRadians(transform->getRotationLocal()));
        float _s = std::sin(degreesToRadians(transform->getRotationLocal()));
        return { _c, -_s, _s, _c };
    }

}