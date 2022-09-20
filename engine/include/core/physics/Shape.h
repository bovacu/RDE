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

#ifndef SHAPE_H
#define SHAPE_H

#include "Body.h"
#include "core/render/shapes/DebugShape.h"
#include "IEMath.h"

namespace Physics {

#define MaxPolyVertexCount 64

    struct Shape
    {
        enum Type
        {
            eCircle,
            ePoly,
            eCount
        };

        Shape( ) {}
        virtual Shape *Clone( void ) const = 0;
        virtual void Initialize( void ) = 0;
        virtual void ComputeMass( real density ) = 0;
        virtual void SetOrient( real radians ) = 0;
        virtual GDE::DebugShape Draw( void ) const = 0;
        virtual Type GetType( void ) const = 0;
        virtual ~Shape() {  }

        Body *body;

        // For circle shape
        real radius;

        // For Polygon shape
        Mat2 u; // Orientation matrix from model to world
    };

    struct Circle : public Shape
    {
        Circle( real r )
        {
            radius = r;
        }

        Shape *Clone( void ) const
        {
            return new Circle( radius );
        }

        void Initialize( void )
        {
            ComputeMass( 1.0f );
        }

        void ComputeMass( real density )
        {
            body->m = PI * radius * radius * density;
            body->im = (body->m) ? 1.0f / body->m : 0.0f;
            body->I = body->m * radius * radius;
            body->iI = (body->I) ? 1.0f / body->I : 0.0f;
        }

        void SetOrient( real radians )
        {
        }

        GDE::DebugShape Draw( void ) const
        {
            return GDE::DebugShape {};
        }

        Type GetType( void ) const
        {
            return eCircle;
        }
    };

    struct PolygonShape : public Shape
    {
        void Initialize( void )
        {
            ComputeMass( 1.0f );
        }

        Shape *Clone( void ) const
        {
            PolygonShape *poly = new PolygonShape( );
            poly->u = u;
            for(uint32 i = 0; i < m_vertexCount; ++i)
            {
                poly->m_vertices.push_back(m_vertices[i]);
                poly->m_normals.push_back(m_normals[i]);
            }
            poly->m_vertexCount = m_vertexCount;
            return poly;
        }

        void ComputeMass( real density )
        {
            // Calculate centroid and moment of interia
            GDE::Vec2F c( 0.0f, 0.0f ); // centroid
            real area = 0.0f;
            real I = 0.0f;
            const real k_inv3 = 1.0f / 3.0f;

            for(uint32 i1 = 0; i1 < m_vertexCount; ++i1)
            {
                // Triangle vertices, third vertex implied as (0, 0)
                GDE::Vec2F p1( m_vertices[i1] );
                uint32 i2 = i1 + 1 < m_vertexCount ? i1 + 1 : 0;
                GDE::Vec2F p2( m_vertices[i2] );

                real D = Cross( p1, p2 );
                real triangleArea = 0.5f * D;

                area += triangleArea;

                // Use area to weight the centroid average, not just vertex position
                c += triangleArea * k_inv3 * (p1 + p2);

                real intx2 = p1.x * p1.x + p2.x * p1.x + p2.x * p2.x;
                real inty2 = p1.y * p1.y + p2.y * p1.y + p2.y * p2.y;
                I += (0.25f * k_inv3 * D) * (intx2 + inty2);
            }

            c *= 1.0f / area;

            // Translate vertices to centroid (make the centroid (0, 0)
            // for the polygon in model space)
            // Not really necessary, but I like doing this anyway
            for(uint32 i = 0; i < m_vertexCount; ++i)
                m_vertices[i] -= c;

            body->m = density * area;
            body->im = (body->m) ? 1.0f / body->m : 0.0f;
            body->I = I * density;
            body->iI = body->I ? 1.0f / body->I : 0.0f;
        }

        void SetOrient( real radians )
        {
            u.Set( radians );
        }

        GDE::DebugShape Draw( void ) const
        {
            GDE::DebugShape _shape;
            for(auto& _vertex : m_vertices) {
                _shape.addPoint({_vertex.x + body->position.x, _vertex.y + body->position.y});
            }
            _shape.showInnerColor(false);
            return _shape;
        }

        Type GetType( void ) const
        {
            return ePoly;
        }

        // Half width and half height
        void SetBox( real hw, real hh )
        {
            m_vertexCount = 4;
            m_vertices.emplace_back( -hw, -hh );
            m_vertices.emplace_back(  hw, -hh );
            m_vertices.emplace_back(  hw,  hh );
            m_vertices.emplace_back( -hw,  hh );
            m_normals.emplace_back(  0.0f,  -1.0f );
            m_normals.emplace_back(  1.0f,   0.0f );
            m_normals.emplace_back(  0.0f,   1.0f );
            m_normals.emplace_back( -1.0f,   0.0f );
        }

        void Set( GDE::Vec2F *vertices, uint32 count )
        {
            // No hulls with less than 3 vertices (ensure actual polygon)
            assert( count > 2 && count <= MaxPolyVertexCount );
            count = std::min( (int32)count, MaxPolyVertexCount );

            // Find the right most point on the hull
            int32 rightMost = 0;
            real highestXCoord = vertices[0].x;
            for(uint32 i = 1; i < count; ++i)
            {
                real x = vertices[i].x;
                if(x > highestXCoord)
                {
                    highestXCoord = x;
                    rightMost = i;
                }

                    // If matching x then take farthest negative y
                else if(x == highestXCoord)
                    if(vertices[i].y < vertices[rightMost].y)
                        rightMost = i;
            }

            int32 hull[MaxPolyVertexCount];
            int32 outCount = 0;
            int32 indexHull = rightMost;

            for (;;)
            {
                hull[outCount] = indexHull;

                // Search for next index that wraps around the hull
                // by computing cross products to find the most counter-clockwise
                // vertex in the set, given the previos hull index
                int32 nextHullIndex = 0;
                for(int32 i = 1; i < (int32)count; ++i)
                {
                    // Skip if same coordinate as we need three unique
                    // points in the set to perform a cross product
                    if(nextHullIndex == indexHull)
                    {
                        nextHullIndex = i;
                        continue;
                    }

                    // Cross every set of three unique vertices
                    // Record each counter clockwise third vertex and add
                    // to the output hull
                    // See : http://www.oocities.org/pcgpe/math2d.html
                    GDE::Vec2F e1 = vertices[nextHullIndex] - vertices[hull[outCount]];
                    GDE::Vec2F e2 = vertices[i] - vertices[hull[outCount]];
                    real c = Cross( e1, e2 );
                    if(c < 0.0f)
                        nextHullIndex = i;

                    // Cross product is zero then e vectors are on same line
                    // therefor want to record vertex farthest along that line
                    if(c == 0.0f && e2.magnitudeSqr( ) > e1.magnitudeSqr( ))
                        nextHullIndex = i;
                }

                ++outCount;
                indexHull = nextHullIndex;

                // Conclude algorithm upon wrap-around
                if(nextHullIndex == rightMost)
                {
                    m_vertexCount = outCount;
                    break;
                }
            }

            // Copy vertices into shape's vertices
            for(uint32 i = 0; i < m_vertexCount; ++i)
                m_vertices.push_back(vertices[hull[i]]);

            // Compute face normals
            for(uint32 i1 = 0; i1 < m_vertexCount; ++i1)
            {
                uint32 i2 = i1 + 1 < m_vertexCount ? i1 + 1 : 0;
                GDE::Vec2F face = m_vertices[i2] - m_vertices[i1];

                // Ensure no zero-length edges, because that's bad
                assert( face.magnitudeSqr( ) > EPSILON * EPSILON );

                // Calculate normal with 2D cross product between vector and scalar
                m_normals.emplace_back( face.y, -face.x );
                m_normals[i1].normalize();
            }
        }

        // The extreme point along a direction within a polygon
        GDE::Vec2F GetSupport( const GDE::Vec2F& dir )
        {
            real bestProjection = -FLT_MAX;
            GDE::Vec2F bestVertex;

            for(uint32 i = 0; i < m_vertexCount; ++i)
            {
                GDE::Vec2F v = m_vertices[i];
                real projection = Dot( v, dir );

                if(projection > bestProjection)
                {
                    bestVertex = v;
                    bestProjection = projection;
                }
            }

            return bestVertex;
        }

        uint32 m_vertexCount;
        std::vector<GDE::Vec2F> m_vertices;
        std::vector<GDE::Vec2F> m_normals;
    };

}

#endif // SHAPE_H
