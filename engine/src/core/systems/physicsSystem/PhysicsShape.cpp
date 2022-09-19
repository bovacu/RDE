//
// Created by borja on 9/16/22.
//

#include <cfloat>
#include "glad/glad.h"
#include "core/systems/physicsSystem/PhysicsShape.h"
#include "core/systems/physicsSystem/PhysicsMath.h"
#include "core/systems/physicsSystem/PhysicsBody.h"
#include "core/util/Mat2.h"
#include "core/Core.h"
#include "core/util/Logger.h"

namespace GDE {

//    void PhysicsShape::makeRectangle(const Vec2F& _size) {
//        type = Type::POLYGON;
//
//        auto _halfSize = Vec2F { _size.x / 2.f, _size.y / 2.f };
//        vertices.emplace_back(Vec2F { -_halfSize.x, -_halfSize.y });
//        vertices.emplace_back(Vec2F {  _halfSize.x, -_halfSize.y });
//        vertices.emplace_back(Vec2F {  _halfSize.x,  _halfSize.y });
//        vertices.emplace_back(Vec2F { -_halfSize.x,  _halfSize.y });
//
//        normals.emplace_back(Vec2F {  0.0f,  -1.0f });
//        normals.emplace_back(Vec2F {  1.0f,   0.0f });
//        normals.emplace_back(Vec2F {  0.0f,   1.0f });
//        normals.emplace_back(Vec2F { -1.0f,   0.0f });
//        size = _size;
//    }
//
//    void PhysicsShape::makeCircle(float _radius) {
//        size.x = _radius;
//        size.y = FLT_MIN;
//
//        type = Type::POLYGON;
//    }
//
//    void PhysicsShape::makePolygon(const std::vector<Vec2F>& _vertices) {
//
//        type = Type::POLYGON;
//
//        // No hulls with less than 3 vertices (ensure actual polygon)
//        ENGINE_ASSERT( _vertices.size() > 2 && _vertices.size() <= MAX_VERTICES_PER_POLYGON, "Number of vertices didn't meet requirements")
//        auto _count = std::min((int)_vertices.size(), MAX_VERTICES_PER_POLYGON );
//
//        // Find the right most point on the hull
//        int rightMost = 0;
//        float highestXCoord = _vertices[0].x;
//        for(int i = 1; i < _count; ++i)
//        {
//            float x = _vertices[i].x;
//            if(x > highestXCoord)
//            {
//                highestXCoord = x;
//                rightMost = i;
//            }
//
//                // If matching x then take farthest negative y
//            else if(x == highestXCoord)
//                if(_vertices[i].y < _vertices[rightMost].y)
//                    rightMost = i;
//        }
//
//        int hull[MAX_VERTICES_PER_POLYGON];
//        int outCount = 0;
//        int indexHull = rightMost;
//
//        for (;;)
//        {
//            hull[outCount] = indexHull;
//
//            // Search for next index that wraps around the hull
//            // by computing cross products to find the most counter-clockwise
//            // vertex in the set, given the previous hull index
//            int nextHullIndex = 0;
//            for(int i = 1; i < _count; ++i)
//            {
//                // Skip if same coordinate as we need three unique
//                // points in the set to perform a cross product
//                if(nextHullIndex == indexHull)
//                {
//                    nextHullIndex = i;
//                    continue;
//                }
//
//                // Cross every set of three unique vertices
//                // Record each counter clockwise third vertex and add
//                // to the output hull
//                // See : http://www.oocities.org/pcgpe/math2d.html
//                Vec2F e1 = _vertices[nextHullIndex] - _vertices[hull[outCount]];
//                Vec2F e2 = _vertices[i] - _vertices[hull[outCount]];
//                float c = cross( e1, e2 );
//                if(c < 0.0f)
//                    nextHullIndex = i;
//
//                // Cross product is zero then e vectors are on same line
//                // therefor want to record vertex farthest along that line
//                if(c == 0.0f && e2.magnitudeSqr( ) > e1.magnitudeSqr( ))
//                    nextHullIndex = i;
//            }
//
//            ++outCount;
//            indexHull = nextHullIndex;
//
//            // Conclude algorithm upon wrap-around
//            if(nextHullIndex == rightMost)
//            {
//                vertexCount = outCount;
//                break;
//            }
//        }
//
//        // Copy vertices into shape's vertices
//        for(int i = 0; i < vertexCount; ++i)
//            vertices.push_back(_vertices[hull[i]]);
//
//        // Compute face normals
//        normals.reserve(vertexCount);
//        for(int i1 = 0; i1 < vertexCount; ++i1)
//        {
//            int i2 = i1 + 1 < vertexCount ? i1 + 1 : 0;
//            Vec2F face = vertices[i2] - vertices[i1];
//
//            // Ensure no zero-length edges, because that's bad
//            ENGINE_ASSERT( face.magnitudeSqr() > EPSILON * EPSILON, "There cannot be zero-length edges!");
//
//            // Calculate normal with 2D cross product between vector and scalar
//            normals.emplace_back(Vec2F { face.y, -face.x });
//            normals.back().normalize();
//        }
//    }
//
//    PhysicsShape* PhysicsShape::clone() {
//        auto* poly = new PhysicsShape( );
//        for(uint32 i = 0; i < vertexCount; ++i)
//        {
//            poly->vertices[i] = vertices[i];
//            poly->normals[i] = normals[i];
//        }
//        poly->vertexCount = vertexCount;
//        poly->transform = transform;
//        return poly;
//    }
//
//    Vec2F PhysicsShape::getSupport(const Vec2F& dir) {
//        real bestProjection = -FLT_MAX;
//        Vec2F bestVertex;
//
//        for(uint32 i = 0; i < vertexCount; ++i)
//        {
//            Vec2 v = vertices[i];
//            real projection = dot( v, dir );
//
//            if(projection > bestProjection)
//            {
//                bestVertex = v;
//                bestProjection = projection;
//            }
//        }
//
//        return bestVertex;
//    }
//
//    void PhysicsShape::setOrient(float _degrees) {
//        u.rotate(_degrees);
//    }
//
//    DebugShape PhysicsShape::debugDraw() {
//        if(type == POLYGON) {
////            glColor3f( physicsBody->r, physicsBody->g, physicsBody->b );
////            glBegin( GL_LINE_LOOP );
////            for(uint32 i = 0; i < vertexCount; ++i)
////            {
////                Vec2 v = physicsBody->position + u * vertices[i];
////                glVertex2f( v.x, v.y );
////            }
////            glEnd( );
//            DebugShape _shape;
//            for(auto& _vertex : vertices) {
//                _shape.addPoint(_vertex + physicsBody->position);
//            }
//            _shape.showInnerColor(false);
//            return _shape;
//        } else {
//            const uint32 k_segments = 20;
//
//            // Render a circle with a bunch of lines
//            glColor3f( physicsBody->r, physicsBody->g, physicsBody->b );
//            glBegin( GL_LINE_LOOP );
//            real theta = physicsBody->orient;
//            real inc = PI * 2.0f / (real)k_segments;
//            for(uint32 i = 0; i < k_segments; ++i)
//            {
//                theta += inc;
//                Vec2 p( std::cos( theta ), std::sin( theta ) );
//                p *= size.x;
//                p += physicsBody->position;
//                glVertex2f( p.x, p.y );
//            }
//            glEnd( );
//
//            // Render line within circle so orientation is visible
//            glBegin( GL_LINE_STRIP );
//            Vec2F r( 0, 1.0f );
//            real c = std::cos( physicsBody->orient );
//            real s = std::sin( physicsBody->orient );
//            r = { r.x * c - r.y * s, r.x * s + r.y * c };
//            r *= size.x;
//            r = r + physicsBody->position;
//            glVertex2f( physicsBody->position.x, physicsBody->position.y );
//            glVertex2f( r.x, r.y );
//            glEnd( );
//        }
//
//        return DebugShape {};
//    }


}